#ifndef STUFF_UTIL
#define STUFF_UTIL

#include <bf2mods/Logger.hpp>
#include <type_traits>

#include "nn/ro.h"
#include "skylaunch/inlinehook/And64InlineHook.hpp"
#include "skylaunch/nx/arm/counter.h"

namespace util {

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#if NOLOG
	#define LOG(...)
#else
	#define LOG(fmt, ...) \
		::xenomods::g_Logger->LogDebug("[{}]: " fmt, __PRETTY_FUNCTION__ __VA_OPT__(, ) __VA_ARGS__);
#endif

	namespace detail {

		void nnRandBase(void* input, std::size_t size);

	} // namespace detail


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

} // namespace util

#endif