#include "sh61.hh"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <vector>
// For the love of God
#undef exit
#define exit __DO_NOT_CALL_EXIT__READ_PROBLEM_SET_DESCRIPTION__

// struct command
//    Data structure describing a command, we'll handle pipeline logic in run()

struct command {
  std::vector<std::string> args;
  command *next_in_pipeline;
  command *prev_in_pipeline;
  pid_t pid = -1; // process ID running this command, -1 if none

  command();
  ~command();

  void run();
};

// struct pipeline
//    Data structure describing a pipeline, we'll handle conditional logic in
//    run()
struct pipeline {
  command *command_child = nullptr;
  pipeline *next_in_conditional = nullptr;
  bool next_is_or = false;

  void run();
};

// struct conditional
//  Data structure describing a conditional, we'll handle background/foreground
//  logic in run()
struct conditional {
  pipeline *pipeline_child = nullptr;
  conditional *next_in_list = nullptr;
  bool is_background = false;

  void run();
};

void pipeline::run() {
  pipeline *p = this;
  bool execute = true;
  int status;
  while (p != nullptr) {
    if (execute) {
      p->command_child->run();
      waitpid(p->command_child->pid, &status, 0);
    }
    if (!p->next_is_or) {
      if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        execute = true;
      } else {
        execute = false;
      }
    } else {
      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        execute = true;
      } else {
        execute = false;
      }
    }
    p = p->next_in_conditional;
  }
}

void conditional::run() {
  conditional *cond = this;
  while (cond != nullptr) {
    pipeline *p = cond->pipeline_child;
    if (!this->is_background) {
      p->run();
      // int status;
      // waitpid(p->command_child->pid, &status,
      //        0); /*  i'm gonna need to change this to wait for the last
      //        command
      //             in the pipeline structure (last conditional)*/
      cond = cond->next_in_list;
    }
  }
}

// command::command()
//    This constructor function initializes a `command` structure. You may
//    add stuff to it as you grow the command structure.

command::command() {}

// command::~command()
//    This destructor function is called to delete a command.

command::~command() {}

// COMMAND EXECUTION

// command::run()
//    Ceates a single child process running the command in `this`, and
//    sets `this->pid` to the pid of the child process.
//
//    If a child process cannot be created, this function should call
//    `_exit(EXIT_FAILURE)` (that is, `_exit(1)`) to exit the containing
//    shell or subshell. If this function returns to its caller,
//    `this->pid > 0` must always hold.
//
//    Note that this function must return to its caller *only* in the parent
//    process. The code that runs in the child process must `execvp` and/or
//    `_exit`.
//
//    PART 1: Fork a child process and run the command using `execvp`.
//       This will require creating a vector of `char*` arguments using
//       `this->args[N].c_str()`. Note that the last element of the vector
//       must be a `nullptr`.
//    PART 4: Set up a pipeline if appropriate. This may require creating a
//       new pipe (`pipe` system call), and/or replacing the child process's
//       standard input/output with parts of the pipe (`dup2` and `close`).
//       Draw pictures!
//    PART 7: Handle redirections.

void command::run() {
  assert(this->pid == -1);
  assert(this->args.size() > 0);
  // Your code here!
  this->pid = fork();
  if (this->pid == 0) {
    int size = this->args.size();
    char **argv;
    argv = new char *[size + 1];
    for (int i = 0; i < size; i++) {
      argv[i] = new char[this->args[i].length() + 1];
      std::strcpy(argv[i], this->args[i].c_str());
    }
    argv[size] = nullptr;
    execvp(argv[0], argv);
    _exit(EXIT_FAILURE);
  }
  // fprintf(stderr, "command::run not done yet\n");
}

// run_list(c)
//    Run the command *list* starting at `c`. Initially this just calls
//    `c->run()` and `waitpid`; you’ll extend it to handle command lists,
//    conditionals, and pipelines.
//
//    It is possible, and not too ugly, to handle lists, conditionals,
//    *and* pipelines entirely within `run_list`, but many students choose
//    to introduce `run_conditional` and `run_pipeline` functions that
//    are called by `run_list`. It’s up to you.
//
//    PART 1: Start the single command `c` with `c->run()`,
//        and wait for it to finish using `waitpid`.
//    The remaining parts may require that you change `struct command`
//    (e.g., to track whether a command is in the background)
//    and write code in `command::run` (or in helper functions).
//    PART 2: Introduce a loop to run a list of commands, waiting for each
//       to finish before going on to the next.
//    PART 3: Change the loop to handle conditional chains.
//    PART 4: Change the loop to handle pipelines. Start all processes in
//       the pipeline in parallel. The status of a pipeline is the status of
//       its LAST command.
//    PART 5: Change the loop to handle background conditional chains.
//       This may require adding another call to `fork()`!

void run_list(conditional *c) {

  c->run();
  // fprintf(stderr, "command::run not done yet\n");
}

// I defined this
// void run_pipeline(pipeline *p) {}

// I defined this
void run_conditional(pipeline *p) {}

// parse_line(s)
//    Parse the command list in `s` and return it. Returns `nullptr` if
//    `s` is empty (only spaces). You’ll extend it to handle more token
//    types.

conditional *parse_line(const char *s) {
  shell_parser parser(s);
  // Your code here!

  // Build the command
  // The handout code treats every token as a normal command word.
  // You'll add code to handle operators.
  command *ccur = nullptr, *next_command = nullptr;
  pipeline *next_pipeline, *pcur = nullptr;
  conditional *next_cond, *condcur, *condhead = nullptr;
  (void)ccur, (void)pcur, (void)condcur;
  shell_token_iterator tempIt = parser.begin();

  for (shell_token_iterator it = parser.begin(); it != parser.end(); ++it) {
    if (!ccur) {
      ccur = new command;
      condcur = new conditional;
      pcur = new pipeline;
      pcur->command_child = ccur;
      condcur->pipeline_child = pcur;
      condhead = condcur;
    }
    switch (it.type()) {
    case TYPE_SEQUENCE:
      tempIt = it; /* this variable is to prevent creating a new empty command,
                      (this happens if ; is the last token of the commandline)
                      this would cause our run_list to fail while trying to
                      execute an unitialized command */
      ++tempIt;
      if (tempIt != parser.end()) {
        next_cond = new conditional;
        next_command = new command;
        next_pipeline = new pipeline;
        condcur->next_in_list = next_cond;
        next_cond->pipeline_child = next_pipeline;
        next_pipeline->command_child = next_command;
        ccur = next_command;
        condcur = next_cond;
        pcur = next_pipeline;
      }
      break;
    case TYPE_AND:
      next_pipeline = new pipeline;
      next_command = new command;
      pcur->next_in_conditional = next_pipeline;
      next_pipeline->command_child = next_command;
      ccur = next_command;
      pcur = next_pipeline;
      break;
    case TYPE_OR:
      next_pipeline = new pipeline;
      next_command = new command;
      pcur->next_in_conditional = next_pipeline;
      next_pipeline->command_child = next_command;
      ccur = next_command;
      pcur->next_is_or = true;
      pcur = next_pipeline;
      break;
    default:
      ccur->args.push_back(it.str());
      break;
    }

    // p->command_child = c;
    // cond->pipeline_child = p;
  }
  return condhead;
}

int main(int argc, char *argv[]) {
  FILE *command_file = stdin;
  bool quiet = false;

  // Check for `-q` option: be quiet (print no prompts)
  if (argc > 1 && strcmp(argv[1], "-q") == 0) {
    quiet = true;
    --argc, ++argv;
  }

  // Check for filename option: read commands from file
  if (argc > 1) {
    command_file = fopen(argv[1], "rb");
    if (!command_file) {
      perror(argv[1]);
      return 1;
    }
  }

  // - Put the shell into the foreground
  // - Ignore the SIGTTOU signal, which is sent when the shell is put back
  //   into the foreground
  claim_foreground(0);
  set_signal_handler(SIGTTOU, SIG_IGN);

  char buf[BUFSIZ];
  int bufpos = 0;
  bool needprompt = true;

  while (!feof(command_file)) {
    // Print the prompt at the beginning of the line
    if (needprompt && !quiet) {
      printf("sh61[%d]$ ", getpid());
      fflush(stdout);
      needprompt = false;
    }

    // Read a string, checking for error or EOF
    if (fgets(&buf[bufpos], BUFSIZ - bufpos, command_file) == nullptr) {
      if (ferror(command_file) && errno == EINTR) {
        // ignore EINTR errors
        clearerr(command_file);
        buf[bufpos] = 0;
      } else {
        if (ferror(command_file)) {
          perror("sh61");
        }
        break;
      }
    }

    // If a complete command line has been provided, run it
    bufpos = strlen(buf);
    if (bufpos == BUFSIZ - 1 || (bufpos > 0 && buf[bufpos - 1] == '\n')) {
      if (conditional *cond = parse_line(buf)) {
        /* for test : for (auto elt : c->args)
          std::cout << elt << "\n"; */
        run_list(cond);
        delete cond;
      }
      bufpos = 0;
      needprompt = 1;
    }

    // Handle zombie processes and/or interrupt requests
    // Your code here!
  }

  return 0;
}
