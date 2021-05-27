#pragma once

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

	/**
 	 * Resolve a symbol of to a pointer-to-function-pointer of the given type.
 	 * 
 	 * \tparam F Function pointer type.
 	 * \param[out] ptr pointer to write to
 	 * \param[in] name Symbol name to resolve.
 	 */
	template<class F>
	inline void ResolveSymbol(F* ptr, const char* name) {
		uintptr_t addr;

		if(R_SUCCEEDED(nn::ro::LookupSymbol(&addr, name))) {
			LOG("Resolved \"%s\" successfully to %p\n", name, addr);
		} else {
			LOG("Failed to look \"%s\" up!\n", name);
			addr = 0xDEADDEAD;
		}

		*ptr = (F)addr;
	}

	/**
 	 * Generic random which uses the nn::os utilities for random bytes.
 	 * 
 	 * \tparam T Type to make random.
 	 */
	template<class T>
	inline T nnRand() {
		T v;
		nn::os::GenerateRandomBytes(&v, sizeof(T));
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
		FpsLogger()
			: m_lastSecondTick(armGetSystemTick()) {
		}

		void tick() {
			auto curTick = armGetSystemTick();
			if(curTick - m_lastSecondTick > armGetSystemTickFreq()) {
				LOG("%d fps", m_frameCount);
				m_frameCount = 0;
				m_lastSecondTick = armGetSystemTick();
			} else {
				m_frameCount++;
			}
		}
	};
#define LOG_FPS                                   \
	static auto s_fpsLogger = util::FpsLogger {}; \
	s_fpsLogger.tick();

} // namespace util
