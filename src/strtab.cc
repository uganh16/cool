#include "strtab.h"

Symbol Strtab::intern_impl(const std::string& str) {
  auto it = symbols.find(str);
  if (it == symbols.end()) {
    it = symbols.emplace(str).first;
  }
  return Symbol(*it);
}
