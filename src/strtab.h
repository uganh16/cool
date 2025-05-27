#pragma once

#include <string>
#include <string_view>
#include <ostream>
#include <unordered_set>

class Symbol {
  friend class Strtab;

public:
  Symbol(void) = default;

  bool operator==(const Symbol& other) const {
    return sv.data() == other.sv.data();
  }

  bool operator!=(const Symbol& other) const {
    return !operator==(other);
  }

  std::string_view view(void) const {
    return sv;
  }

  std::string to_string(void) const {
    return std::string(sv);
  }

private:
  explicit Symbol(std::string_view sv)
    : sv(sv) {}

  std::string_view sv;
};

inline std::ostream& operator<<(std::ostream& os, const Symbol& str) {
  os << str.view();
  return os;
}

template<>
struct std::hash<Symbol> {
  size_t operator()(const Symbol& s) const noexcept {
    return std::hash<const char*>()(s.view().data());
  }
};

class Strtab {
public:
  static Symbol intern(const std::string& str) {
    static Strtab strtab;
    return strtab.intern_impl(str);
  }

private:
  Strtab(void) = default;

  Symbol intern_impl(const std::string& str);

  std::unordered_set<std::string> symbols;
};
