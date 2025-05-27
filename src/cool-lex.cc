#include "cool-lex.h"

LexState::LexState(std::istream& in)
  : buffer(
      std::istreambuf_iterator<char>(in),
      std::istreambuf_iterator<char>())
  , cursor(buffer.data())
  , limit(cursor + buffer.size())
  , line_start(cursor)
  , curr_line(1) {

}

void LexState::skip_single_line_comment(void) {
  /* Skip line comment. */
  while (cursor < limit) {
    if (*cursor++ == '\n') {
      newline();
      break;
    }
  }
}

bool LexState::skip_block_comment(void) {
  int nest_level = 1; /* Block comment may be nested. */
  while (cursor < limit) {
    switch (*cursor++) {
      case '\n':
        newline();
        break;
      case '(':
        if (*cursor == '*') {
          ++cursor;
          ++nest_level;
        }
        break;
      case '*':
        if (*cursor == ')') {
          ++cursor;
          if (--nest_level == 0) {
            return true;
          }
        }
        break;
    }
  }
  return false;
}

bool LexState::read_string_literal(Value* yylval_ptr) {
  std::string string_literal;

  while (cursor < limit) {
    char chr = *cursor++;
    if (chr == '\0') {
      yylval_ptr->emplace<std::string>("String contains null character");
      goto bad_string;
    } else if (chr == '\n') {
      newline();
      yylval_ptr->emplace<std::string>("Unterminated string constant");
      return false;
    } else if (chr == '"') {
      yylval_ptr->emplace<std::string>(string_literal);
      return true;
    } else if (chr == '\\') {
      if (cursor == limit) {
        yylval_ptr->emplace<std::string>("Backslash at end of file");
        return false;
      }
      switch (chr = *cursor++) {
        case '\0':
          yylval_ptr->emplace<std::string>("String contains null character");
          goto bad_string;
        case '\n':
          newline();
          string_literal.push_back('\n');
          break;
        case 'b': string_literal.push_back('\b'); break;
        case 'f': string_literal.push_back('\f'); break;
        case 'n': string_literal.push_back('\n'); break;
        case 't': string_literal.push_back('\t'); break;
        default:
          string_literal.push_back(chr);
          break;
      }
    } else {
      string_literal.push_back(chr);
    }
  }

  yylval_ptr->emplace<std::string>("EOF in string constant");

bad_string:
  /* The end of the string is defined as either:
   * 1. the beginning of the next line if an unescaped newline occurs at any point in the string; or
   * 2. after the closing " otherwise. */
  while (cursor < limit) {
    char chr = *cursor++;
    if (chr == '\n') {
      newline();
      break;
    } else if (chr == '\\') {
      if (cursor < limit) {
        if (*cursor++ == '\n') {
          newline();
        }
      }
    }
  }

  return false;
}
