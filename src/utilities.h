#pragma once

#include <ostream>
#include <string_view>

#include "cool-lex.h"

class EscapedString {
public:
  explicit EscapedString(std::string_view sv)
    : sv(sv) {}

  friend std::ostream& operator<<(std::ostream& out, const EscapedString& escaped);

private:
  std::string_view sv;
};

inline EscapedString escape(std::string_view sv) {
  return EscapedString(sv);
}

void print_token(std::ostream& out, int token, const LexState::Value& yylval, const LexState::Location& yylloc);
