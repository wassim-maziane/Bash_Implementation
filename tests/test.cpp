#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
// method 1 : tree structure
struct command {
  std::vector<std::string> args;
  command *next_in_pipeline = nullptr;
  command *prev_in_pipeline = nullptr;
};

struct pipeline {
  command *command_child = nullptr;
  pipeline *next_in_conditional = nullptr;
  bool next_is_or = false;
};

struct conditional {
  pipeline *pipeline_child = nullptr;
  conditional *next_in_list = nullptr;
  bool is_background = false;
};

// method 2 : linked list
struct command2 {
  std::vector<std::string> args;
  command *next_command = nullptr;
  command *prev_command = nullptr;
  int next_op;
};

command *parse_line(const char *s) {
  std::istringstream stream(s);
  std::string word;
  command *chead = nullptr;
  command *clast = nullptr;
  command *ccur = nullptr;
  while (std::cin >> word) {
    switch ()
  }
}
