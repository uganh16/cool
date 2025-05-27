#include "utilities.h"

std::ostream& operator<<(std::ostream& out, const EscapedString& escaped) {
  static const char* hex_chars = "0123456789abcdef";
  for (char chr : escaped.sv) {
    switch (chr) {
      case '\b': out << "\\b"; break;
      case '\t': out << "\\t"; break;
      case '\n': out << "\\n"; break;
      case '\f': out << "\\f"; break;
      case '\"': out << "\\\""; break;
      case '\\': out << "\\\\"; break;
      default:
        if (std::isprint(chr)) {
          out << chr;
        } else {
          out << "\\x"
              << hex_chars[(static_cast<unsigned char>(chr) >> 4) & 0x0f]
              << hex_chars[ static_cast<unsigned char>(chr) & 0x0f];
        }
        break;
    }
  }
  return out;
}

const char *token_to_string(int token) {
  switch (token) {
    case TOKID(END):      return "EOF";
    case '(':             return "'('";
    case ')':             return "')'";
    case '*':             return "'*'";
    case '+':             return "'+'";
    case ',':             return "','";
    case '-':             return "'-'";
    case '.':             return "'.'";
    case '/':             return "'/'";
    case ':':             return "':'";
    case ';':             return "';'";
    case '<':             return "'<'";
    case '=':             return "'='";
    case '@':             return "'@'";
    case '{':             return "'{'";
    case '}':             return "'}'";
    case '~':             return "'~'";
    case TOKID(CASE):     return "CASE";
    case TOKID(CLASS):    return "CLASS";
    case TOKID(ELSE):     return "ELSE";
    case TOKID(ESAC):     return "ESAC";
    case TOKID(FALSE):    return "BOOL_CONST";
    case TOKID(FI):       return "FI";
    case TOKID(IF):       return "IF";
    case TOKID(IN):       return "IN";
    case TOKID(INHERITS): return "INHERITS";
    case TOKID(ISVOID):   return "ISVOID";
    case TOKID(LET):      return "LET";
    case TOKID(LOOP):     return "LOOP";
    case TOKID(NEW):      return "NEW";
    case TOKID(NOT):      return "NOT";
    case TOKID(OF):       return "OF";
    case TOKID(POOL):     return "POOL";
    case TOKID(THEN):     return "THEN";
    case TOKID(TRUE):     return "BOOL_CONST";
    case TOKID(WHILE):    return "WHILE";
    case TOKID(NUMBER):   return "INT_CONST";
    case TOKID(STRING):   return "STR_CONST";
    case TOKID(OBJECTID): return "OBJECTID";
    case TOKID(TYPEID):   return "TYPEID";
    case TOKID(ASSIGN):   return "ASSIGN";
    case TOKID(DARROW):   return "DARROW";
    case TOKID(LE):       return "LE";
    case TOKID(ERROR):    return "ERROR";
    default:              return "<Invalid Token>";
  }
}

void print_token(std::ostream& out, int token, const LexState::Value& yylval, const LexState::Location& yylloc) {
  out << "#" << yylloc.line << " " << token_to_string(token);
  switch (token) {
    case TOKID(TRUE):
      out << " true";
      break;
    case TOKID(FALSE):
      out << " false" << std::endl;
      break;
    case TOKID(NUMBER):
      out << " " << std::get<long>(yylval);
      break;
    case TOKID(STRING):
      out << " \"" << escape(std::get<std::string>(yylval)) << "\"";
      break;
    case TOKID(OBJECTID):
    case TOKID(TYPEID):
      out << " " << std::get<Symbol>(yylval);
      break;
    case TOKID(ERROR):
      out << " \"" << std::get<std::string>(yylval) << "\"";
      break;
  }
  out << std::endl;
}
