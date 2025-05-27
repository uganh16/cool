#include "cool-lex.h"

#include <cstdlib>

#define yyleng static_cast<size_t>(cursor - yytext)

#define YIELD(name)                                                            \
  do {                                                                         \
    yylloc_ptr->line = curr_line;                                              \
    yylloc_ptr->column = static_cast<unsigned int>(cursor - line_start);       \
    return TOKID(name);                                                        \
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
