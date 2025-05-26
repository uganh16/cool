#pragma once

#include <istream>
#include <string>
#include <variant>

#include "strtab.h"

#define TOKID(name) TOK_##name

enum {
  TOK_END = 0,

  TOK_ERROR = 256,

  TOK_CASE,
  TOK_CLASS,
  TOK_ELSE,
  TOK_ESAC,
  TOK_FALSE,
  TOK_FI,
  TOK_IF,
  TOK_IN,
  TOK_INHERITS,
  TOK_ISVOID,
  TOK_LET,
  TOK_LOOP,
  TOK_NEW,
  TOK_NOT,
  TOK_OF,
  TOK_POOL,
  TOK_THEN,
  TOK_TRUE,
  TOK_WHILE,

  TOK_NUMBER,
  TOK_STRING,

  TOK_OBJECTID,
  TOK_TYPEID,

  TOK_ASSIGN, // "<-"
  TOK_DARROW, // "=>"
  TOK_LE,     // "<="
};

class LexState {
public:
  using Value = std::variant<long, Symbol, std::string>;

  struct Location {
    unsigned int line;
    unsigned int column;
  };

  explicit LexState(std::istream& in);

  LexState(const LexState&) = delete;
  LexState& operator=(const LexState&) = delete;

  int lex(Value* yylval_ptr, Location* yylloc_ptr);

private:
  void skip_single_line_comment(void);
  bool skip_block_comment(void);
  bool read_string_literal(Value* yylval_ptr);

  void newline(void) {
    ++curr_line;
    line_start = cursor;
  }

  std::string buffer;
  const char* cursor;
  const char* limit;

  const char* line_start;
  unsigned int curr_line;
};
