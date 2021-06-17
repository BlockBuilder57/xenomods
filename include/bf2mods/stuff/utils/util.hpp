#ifndef STUFF_UTIL
#define STUFF_UTIL

#include <type_traits>

#include "nn/ro.h"
#include "skyline/efl/service.hpp"
#include "skyline/inlinehook/And64InlineHook.hpp"
#include "skyline/nx/arm/counter.h"

namespace util {

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

#if NOLOG
#define LOG(...)
#else
#if USE_EFL_LOG
#define EFL_LOG_BUFFER_SIZE 0x1000
	static auto s_eflLogBuffer = std::array<char, EFL_LOG_BUFFER_SIZE> { 0 };

#define LOG(fmt, ...)                                                            \
			snprintf(util::s_eflLogBuffer.data(), EFL_LOG_BUFFER_SIZE - 1, "[%s]: " fmt, \
					 __PRETTY_FUNCTION__ __VA_OPT__(, ) __VA_ARGS__);                    \
			skyline::efl::Log("...", EFL_LOG_LEVEL_INFO, util::s_eflLogBuffer.data());

#else
#define LOG(fmt, ...) \
            skyline::logger::s_Instance->LogFormat("[%s]: " fmt, __PRETTY_FUNCTION__ __VA_OPT__(, ) __VA_ARGS__);

#endif
#endif

	namespace detail {

		/**
		 * Base version of ResolveSymbol.
		 */
		bool ResolveBase(void** Output, const char* name);

		void nnRandBase(void* input, std::size_t size);

	}

	/**
 	 * Resolves a given symbol to a pointer-to-pointer of the given type.
 	 * 
 	 * \tparam F Pointer type. Mainly used for functions
 	 * \param[out] ptr Pointer to write to.
 	 * \param[in] name Symbol name to resolve.
 	 */
	template<class F>
	constexpr void ResolveSymbol(F* ptr, const char* name) {
		if (!detail::ResolveBase(reinterpret_cast<void**>(ptr), name)) {
			LOG("ResolveBase(ptr: %p, name: %s) returned false", ptr, name);
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

	// TODO refactor these macros to use ^^^^ ResolveSymbol abstraction

#define GENERATE_SYM_HOOK(name, symbolStr, ReturnType, ...)                                  \
    ReturnType (*name##Bak)(__VA_OPT__(__VA_ARGS__));                                        \
    ReturnType name##Replace(__VA_OPT__(__VA_ARGS__));                                       \
    void name##Hook() {                                                                      \
        uintptr_t symbolAddress;                                                             \
        if(R_SUCCEEDED(nn::ro::LookupSymbol(&symbolAddress, symbolStr))) {                   \
            LOG("Hooking %s...", STRINGIFY(name));                                           \
            A64HookFunction((void*)symbolAddress, (void*)name##Replace, (void**)&name##Bak); \
        } else {                                                                             \
            LOG("Failed to look up %s, symbol is: %s", STRINGIFY(name), symbolStr);          \
        }                                                                                    \
    }                                                                                        \
    ReturnType name##Replace(__VA_OPT__(__VA_ARGS__))

#define CLASS_METHOD_RETURN_TYPE(ClassName, methodName, ...) \
    std::result_of<decltype (&ClassName::methodName)(ClassName __VA_OPT__(, __VA_ARGS__))>::type

#define GENERATE_CLASS_HOOK(ClassName, methodName, ...)                                                      \
    CLASS_METHOD_RETURN_TYPE(ClassName, methodName __VA_OPT__(, __VA_ARGS__))                                \
    (*methodName##Bak)(ClassName * __VA_OPT__(, __VA_ARGS__));                                               \
                                                                                                             \
    CLASS_METHOD_RETURN_TYPE(ClassName, methodName __VA_OPT__(, __VA_ARGS__))                                \
    methodName##Replace(ClassName* p_this __VA_OPT__(, __VA_ARGS__));                                        \
                                                                                                             \
    void methodName##Hook() {                                                                                \
        LOG("hooking %s::%s...", STRINGIFY(ClassName), STRINGIFY(methodName));                               \
        auto methodName##Addr = &ClassName::methodName;                                                      \
        A64HookFunction(*(void**)(&methodName##Addr), (void*)methodName##Replace, (void**)&methodName##Bak); \
    }                                                                                                        \
                                                                                                             \
    CLASS_METHOD_RETURN_TYPE(ClassName, methodName __VA_OPT__(, __VA_ARGS__))                                \
    methodName##Replace(ClassName* p_this __VA_OPT__(, __VA_ARGS__))

#define CLASS_OVERLOADED_METHOD_RETURN_TYPE(ClassName, methodPtr, ...) \
    std::result_of<decltype(methodPtr)(ClassName __VA_OPT__(, __VA_ARGS__))>::type

#define GENERATE_CLASS_HOOK_NAMED(hookName, ClassName, methodName, ...)                                   \
    auto (ClassName::*hookName##Addr)(__VA_ARGS__) = &ClassName::methodName;                              \
                                                                                                          \
    CLASS_OVERLOADED_METHOD_RETURN_TYPE(ClassName, hookName##Addr __VA_OPT__(, __VA_ARGS__))              \
    (*hookName##Bak)(ClassName * __VA_OPT__(, __VA_ARGS__));                                              \
                                                                                                          \
    CLASS_OVERLOADED_METHOD_RETURN_TYPE(ClassName, hookName##Addr __VA_OPT__(, __VA_ARGS__))              \
    hookName##Replace(ClassName* p_this __VA_OPT__(, __VA_ARGS__));                                       \
                                                                                                          \
    void hookName##Hook() {                                                                               \
        LOG("hooking %s::%s to %s...", STRINGIFY(ClassName), STRINGIFY(methodName), STRINGIFY(hookName)); \
        A64HookFunction(*(void**)&hookName##Addr, (void*)hookName##Replace, (void**)&hookName##Bak);      \
    }                                                                                                     \
                                                                                                          \
    CLASS_OVERLOADED_METHOD_RETURN_TYPE(ClassName, hookName##Addr __VA_OPT__(, __VA_ARGS__))              \
    hookName##Replace(ClassName* p_this __VA_OPT__(, __VA_ARGS__))

	class FpsLogger {
		uint m_frameCount;
		uint64_t m_lastSecondTick;

	public:
		FpsLogger();

		void tick();
	};

#define UTIL_LOG_FPS                              \
    static auto s_fpsLogger = util::FpsLogger {}; \
    s_fpsLogger.tick();

} // namespace util

#endif