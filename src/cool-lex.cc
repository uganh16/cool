#include "cool-lex.h"

#include <cstdlib>

#define yyleng static_cast<size_t>(cursor - yytext)

LexState::LexState(std::istream& in)
  : buffer(
      std::istreambuf_iterator<char>(in),
      std::istreambuf_iterator<char>())
  , cursor(buffer.data())
  , limit(cursor + buffer.size())
  , line_start(cursor)
  , curr_line(1) {

}

#define YIELD(name)                           \
  do {                                        \
    yylloc_ptr->line = curr_line;             \
    yylloc_ptr->column = cursor - line_start; \
    return TOKID(name);                       \
  } while (0)

int LexState::lex(Value *yylval_ptr, Location *yylloc_ptr) {
  const char *yytext;

loop:
  yytext = cursor;

/*!re2c
  re2c:define:YYCTYPE = char;
  re2c:define:YYCURSOR = cursor;
  re2c:define:YYLIMIT = limit;

  re2c:yyfill:enable = 0;

  digit = [0-9];
  alpha = [A-Za-z];
  ident = digit | alpha | "_";

  * {
    if (cursor > limit) {
      --cursor;
      YIELD(END);
    }
    yylval_ptr->emplace<std::string>(1, yytext[0]);
    YIELD(ERROR);
  }

  [\t\v\f\r ]+ {
    goto loop;
  }

  "\n" {
    newline();
    goto loop;
  }

  "--" {
    skip_single_line_comment();
    goto loop;
  }

  "(*" {
    if (skip_block_comment()) {
      goto loop;
    }
    yylval_ptr->emplace<std::string>("EOF in comment");
    YIELD(ERROR);
  }

  "*)" {
    yylval_ptr->emplace<std::string>("Unmatched *)");
    YIELD(ERROR);
  }

  ["] {
    if (read_string_literal(yylval_ptr)) {
      YIELD(STRING);
    }
    YIELD(ERROR);
  }

  digit+ {
    yylval_ptr->emplace<long>(std::strtol(yytext, NULL, 10));
    YIELD(NUMBER);
  }

  [()*+,-./:;<=@{}~] {
    return yytext[0];
  }

  "<=" {
    YIELD(LE);
  }

  "<-" {
    YIELD(ASSIGN);
  }

  "=>" {
    YIELD(DARROW);
  }

  'case' {
    YIELD(CASE);
  }

  'class' {
    YIELD(CLASS);
  }

  'else' {
    YIELD(ELSE);
  }

  'esac' {
    YIELD(ESAC);
  }

  "f" 'alse' {
    YIELD(FALSE);
  }

  'fi' {
    YIELD(FI);
  }

  'if' {
    YIELD(IF);
  }

  'in' {
    YIELD(IN);
  }

  'inherits' {
    YIELD(INHERITS);
  }

  'isvoid' {
    YIELD(ISVOID);
  }

  'let' {
    YIELD(LET);
  }

  'loop' {
    YIELD(LOOP);
  }

  'new' {
    YIELD(NEW);
  }

  'not' {
    YIELD(NOT);
  }

  'of' {
    YIELD(OF);
  }

  'pool' {
    YIELD(POOL);
  }

  'then' {
    YIELD(THEN);
  }

  "t" 'rue' {
    YIELD(TRUE);
  }

  'while' {
    YIELD(WHILE);
  }

  [A-Z] ident* {
    yylval_ptr->emplace<Symbol>(Strtab::intern(std::string(yytext, yyleng)));
    YIELD(TYPEID);
  }

  [a-z] ident* {
    yylval_ptr->emplace<Symbol>(Strtab::intern(std::string(yytext, yyleng)));
    YIELD(OBJECTID);
  }
 */
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
