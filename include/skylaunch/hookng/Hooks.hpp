// Primary include file for "hook-ng"
#pragma once

#include <nn/ro.h>

#include <skylaunch/utils/cpputils.hpp>
#include <skylaunch/inlinehook/And64InlineHook.hpp>
#include <bit>
#include <string_view>
#include <skylaunch/hookng/FunctionPointers.hpp>

#include <xenomods/Logger.hpp>

namespace skylaunch::hook {

	const uintptr_t INVALID_FUNCTION_PTR = 0xDEADDEAD;

	namespace detail {

		void* HookFunctionBase(void* function, void* replacement);
		void InlineHookBase(void* addr, void* handler);
		uintptr_t ResolveSymbolBase(std::string_view symbolName);

		template<class Func, class FuncHook>
		constexpr Func HookFunction(Func function, FuncHook hook) {
			return std::bit_cast<Func>(HookFunctionBase(std::bit_cast<void*>(function), std::bit_cast<void*>(hook)));
		}

		template<class SymType>
		constexpr SymType ResolveSymbol(std::string_view symbolName) {
			return std::bit_cast<SymType>(ResolveSymbolBase(symbolName));
		}

	} // namespace detail

	/**
	 * A replace hook. (does not save the original function)
	 * \tparam Impl Implementation class.
	 */
	template<class Impl>
	struct Replace {
		template<class DelayedImpl = Impl>
		using ReplaceHookType = decltype(&DelayedImpl::Hook);

		template<class R, class... Args>
		static inline void HookAt(FuncPtr<R, Args...> address) {
			(void)detail::HookFunction(reinterpret_cast<ReplaceHookType<>>(address), &Impl::Hook);
		}

		template<class T, class R, class... Args>
		static inline void HookAt(MemberFuncPtr<T, R, Args...> address) {
			detail::HookFunction(reinterpret_cast<ReplaceHookType<>>(FlattenMemberPtr(address)), &Impl::Hook);
		}

		static inline void HookAt(uintptr_t address) {
			(void)detail::HookFunction(reinterpret_cast<ReplaceHookType<>>(address), &Impl::Hook);
		}

		static inline void HookAt(std::string_view symbolName) {
			if (detail::ResolveSymbolBase(symbolName) != INVALID_FUNCTION_PTR)
				(void)detail::HookFunction(detail::ResolveSymbol<ReplaceHookType<>>(symbolName), &Impl::Hook);
			else
				xenomods::g_Logger->LogWarning("[hook-ng] Failed to hook \"{}\"! Something may break.", symbolName);
		}

		static inline void HookFromBase(uintptr_t address) {
			(void)detail::HookFunction(reinterpret_cast<ReplaceHookType<>>(skylaunch::utils::AddrFromBase(address)), &Impl::Hook);
		}
	};

	/**
	 * A trampoline hook.
	 * \tparam Impl Implementation class.
	 */
	template<class Impl>
	struct Trampoline {
		template<class DelayedImpl = Impl>
		using TrampolineHookType = decltype(&DelayedImpl::Hook);

		static bool HasApplied() {
			return Backup() != nullptr;
		}

		template<class... Args>
		static auto Orig(Args&&... args) {
			return Backup()(static_cast<Args&&>(args)...);
		}

		template<class R, class... Args>
		static inline void HookAt(FuncPtr<R, Args...> address) {
			Backup() = detail::HookFunction(reinterpret_cast<TrampolineHookType<>>(address), &Impl::Hook);
		}

		template<class T, class R, class... Args>
		static inline void HookAt(MemberFuncPtr<T, R, Args...> address) {
			Backup() = detail::HookFunction(reinterpret_cast<TrampolineHookType<>>(FlattenMemberPtr(address)), &Impl::Hook);
		}

		static inline void HookAt(uintptr_t address) {
			Backup() = detail::HookFunction(reinterpret_cast<TrampolineHookType<>>(address), &Impl::Hook);
		}

		static inline void HookAt(std::string_view symbolName) {
			if (detail::ResolveSymbolBase(symbolName) != INVALID_FUNCTION_PTR)
				Backup() = detail::HookFunction(detail::ResolveSymbol<TrampolineHookType<>>(symbolName), &Impl::Hook);
			else
				xenomods::g_Logger->LogWarning("[hook-ng] Failed to hook \"{}\"! Something may break.", symbolName);
		}

		static inline void HookFromBase(uintptr_t address) {
			Backup() = detail::HookFunction(reinterpret_cast<TrampolineHookType<>>(skylaunch::utils::AddrFromBase(address)), &Impl::Hook);
		}

		static auto& Backup() {
			constinit static TrampolineHookType<> backup {};
			return backup;
		}
	};

	// FIXME(lily): Doesn't seem to work quite right at the moment.. Don't know why
	template<class Impl>
	struct InlineHook {
		template<class Delayed = Impl>
		using ImplHookType = decltype(&Delayed::Handler);

		using ExpectedHookType = void(*)(InlineCtx);

		static inline void HookAt(uintptr_t address) {
			static_assert(std::is_same_v<ImplHookType<>, ExpectedHookType>, "need an Handler function please");
			A64InlineHook(reinterpret_cast<void*>(address), reinterpret_cast<void*>(&Impl::Handler));
			//detail::InlineHookBase(reinterpret_cast<void*>(Impl::Handler), reinterpret_cast<void*>(address));
		}
	};

	template<class Sig>
	struct StubHook {};

	/**
	 * A simple replace-hook which tries its best to stub out without much intervention.
	 *
	 * \tparam R Function return type.
	 * \tparam Args Function additional arguments. Not consumed.
	 */
	template<class R, class... Args>
	struct StubHook<R(Args...)> : Replace<StubHook<R(Args...)>> {
		static R Hook(Args... args) {
			if constexpr(std::is_same_v<R, void>)
				return;
			else
				return {};
		}
	};

} // namespace skylaunch::hook