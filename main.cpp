#include <iostream>
#include "parser.h"
#include "utils.h"

int main() {
  std::string input = "CREATE TABLE IF NOT EXISTS helloworld ();";
  Parser parser(input);
  parser.parse();
  return 0;
}
