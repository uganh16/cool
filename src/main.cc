#include <fstream>
#include <iostream>

#include "cool-lex.h"
#include "utilities.h"

int main(int argc, char* argv[]) {
  int opt_index = 1;

  while (opt_index < argc) {
    const char* filename = argv[opt_index++];

    std::ifstream in(filename);
    if (!in.is_open()) {
      std::cerr << "Could not open input file: " << filename << std::endl;
      return -1;
    }

    std::cout << "#name \"" << filename << "\"" << std::endl;
    LexState lexer(in);

    int token;
    LexState::Value yylval;
    LexState::Location yylloc;

    while ((token = lexer.lex(&yylval, &yylloc)) != TOKID(END)) {
      print_token(std::cout, token, yylval, yylloc);
    }

    in.close();
  }

  return 0;
}
