#ifndef STUFF_UTIL
#define STUFF_UTIL

#include <type_traits>

#include "nn/ro.h"
#include "skylaunch/inlinehook/And64InlineHook.hpp"
#include "skylaunch/nx/arm/counter.h"

#include <bf2mods/bf2logger.hpp>

namespace util {

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#if NOLOG
	#define LOG(...)
#else
	#define LOG(fmt, ...) \
		::bf2mods::g_Logger->LogDebug("[{}]: " fmt, __PRETTY_FUNCTION__ __VA_OPT__(, ) __VA_ARGS__);
#endif

	namespace detail {

		/**
		 * Base version of ResolveSymbol.
		 */
		bool ResolveBase(void** Output, const char* name);

		void nnRandBase(void* input, std::size_t size);

	} // namespace detail

	/**
 	 * Resolves a given symbol to a pointer-to-pointer of the given type.
 	 * 
 	 * \tparam F Pointer type. Mainly used for functions
 	 * \param[out] ptr Pointer to write to.
 	 * \param[in] name Symbol name to resolve.
 	 */
	template<class F>
	constexpr void ResolveSymbol(F* ptr, const char* name) {
		if(!detail::ResolveBase(reinterpret_cast<void**>(ptr), name)) {
			LOG("ResolveBase(ptr: {}, name: {}) returned false", reinterpret_cast<void*>(*ptr), name);
		}
	}

	/**
 	 * Generic random which uses the nn::os utilities to get random bytes.
 	 * 
 	 * \tparam T Type to make random.
 	 */
	template<class T>
	constexpr T nnRand() {
		T v;
		detail::nnRandBase(&v, sizeof(T));
		return v;
	}

#define GENERATE_SYM_HOOK(name, symbolStr, ReturnType, ...)                                  \
	ReturnType (*name##Bak)(__VA_OPT__(__VA_ARGS__));                                        \
	ReturnType name##Replace(__VA_OPT__(__VA_ARGS__));                                       \
	void name##Hook() {                                                                      \
		uintptr_t symbolAddress;                                                             \
		if(R_SUCCEEDED(nn::ro::LookupSymbol(&symbolAddress, symbolStr))) {                   \
			LOG("Hooking {}...", STRINGIFY(name));                                           \
			A64HookFunction((void*)symbolAddress, (void*)name##Replace, (void**)&name##Bak); \
		} else {                                                                             \
			LOG("Failed to look up {}, symbol is: {}", STRINGIFY(name), symbolStr);          \
		}                                                                                    \
	}                                                                                        \
	ReturnType name##Replace(__VA_OPT__(__VA_ARGS__))

} // namespace util

#endif