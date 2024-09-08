#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>
using namespace std;
int main() {
  vector<string> argv = {"ls"};
  int size = argv.size();
  int sumlength = 0;
  char **argvcopy;
  int length;
  for (int i = 0; i < size; i++) {
    length = argv[i].length();
    sumlength += length + 1;
  }
  argvcopy = new char *[size + 1];
  for (int i = 0; i < size; i++) {
    length = argv[i].length();
    argvcopy[i] = new char[length + 1];
    strcpy(argvcopy[i], argv[i].c_str());
  }
  argvcopy[size] = nullptr;
  execvp("ls", (char **)argvcopy);
  for (int i = 0; i < size; i++) {
    delete[] argvcopy[i];
  }
}
