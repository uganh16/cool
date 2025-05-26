#pragma once

#include <ostream>

#include "cool-lex.h"

void print_token(std::ostream& out, int token, const LexState::Value& yylval, const LexState::Location& yylloc);

void print_escaped_string(std::ostream& out, const std::string& str);
