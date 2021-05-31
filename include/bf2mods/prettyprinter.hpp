#include <sstream>
#include <string>

namespace bf2mods {

/**
 * A prettyprinter.
 */
template <class T, typename = void> struct prettyprinter {
  using _nope = void;

  // inline static std::string format(const T&) { return "formatted"; }
  // Prettyprints the given value.

  // inline static std::string_view type_name() { return "T"; }
  // Returns the typename of the value.
};

#define _PRETTYPRINTER_GENERATE_SSTREAMABLE(T)                                 \
  template <> struct prettyprinter<T> {                                        \
    inline static std::string format(const T &value) {                         \
      std::stringstream ss;                                                    \
      ss << value;                                                             \
      return ss.str();                                                         \
    }                                                                          \
                                                                               \
    inline static std::string_view type_name() { return #T; }                  \
  };

_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::uint8_t)
_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::int8_t)
_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::uint16_t)
_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::int16_t)
_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::uint32_t)
_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::int32_t)
_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::uint64_t)
_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::int64_t)

_PRETTYPRINTER_GENERATE_SSTREAMABLE(float)
_PRETTYPRINTER_GENERATE_SSTREAMABLE(double)

_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::string)
_PRETTYPRINTER_GENERATE_SSTREAMABLE(std::string_view)

template<>
struct prettyprinter<bool> {

  inline static std::string format(const bool& v) { return (v ? "true" : "false"); }
  inline static std::string_view type_name() { return "bool"; }
  
};

#undef _PRETTYPRINTER_GENERATE_SSTREAMABLE

/**
 * Formats a value such so that it's typename is prepended
 * before the formatted value. Such as "Foo (bar: baz, baz: bar)"
 */
template <class T> inline std::string format_with_typename(const T &t) {
  std::stringstream ss;
  ss << prettyprinter<T>::type_name() << ' ' << prettyprinter<T>::format(t);
  return ss.str();
}
} // namespace bf2mods