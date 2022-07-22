#ifndef BF2MODS_PRETTYPRINTER
#define BF2MODS_PRETTYPRINTER

#include <sstream>
#include <string>

namespace bf2mods {

	// TODO: fmt.

	/**
 * A prettyprinter.
 */
	template<class T, typename = void>
	struct Prettyprinter {
		using _nope = void;

		// inline static std::string format(const T&) { return "formatted"; }
		// Prettyprints the given value, returning a string.

		// inline static std::string_view type_name() { return "T"; }
		// Returns the typename of the value, returning a string_view.
		// This is permitted to be read only, and in fact, should be read only.
	};

// Generate a base implementation for types which are already
// printable with std::stringstream (and ultimately the base IOStreams library)
#define _PRETTYPRINTER_GENERATE_SSTREAMABLE(T)                    \
	template<>                                                    \
	struct Prettyprinter<T> {                                     \
		inline static std::string format(const T& value) {        \
			std::stringstream ss;                                 \
			ss << value;                                          \
			return ss.str();                                      \
		}                                                         \
                                                                  \
		inline static std::string_view type_name() { return #T; } \
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

	/**
 * Specialization of prettyprinter for the bool type.
 */
	template<>
	struct Prettyprinter<bool> {
		inline static std::string format(const bool& v) {
			return (v ? "true" : "false");
		}
		inline static std::string_view type_name() {
			return "bool";
		}
	};

#undef _PRETTYPRINTER_GENERATE_SSTREAMABLE

	/**
 * Formats a value such so that it's typename is prepended
 * before the formatted value. Such as "Foo (bar: baz, baz: bar)"
 */
	template<class T>
	inline std::string format_with_typename(const T& t) {
		std::stringstream ss;
		ss << Prettyprinter<T>::type_name() << ' ' << Prettyprinter<T>::format(t);
		return ss.str();
	}

	/**
 * Plain formats a value.
 * Essentially a fancy wrapper for Prettyprinter<T>::format().
 */
	template<class T>
	inline std::string format(const T& t) {
		return Prettyprinter<T>::format(t);
	}

} // namespace bf2mods

#endif