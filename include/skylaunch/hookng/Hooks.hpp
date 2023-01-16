// Primary include file for "hook-ng"
#pragma once

#include <nn/ro.h>

#include <bf2mods/Logger.hpp>
#include <skylaunch/inlinehook/And64InlineHook.hpp>
#include <string_view>
#include <skylaunch/hookng/FunctionPointers.hpp>

namespace skylaunch::hook {

	namespace detail {

		template<class Func, class FuncHook>
		inline Func HookFunction(Func function, FuncHook hook) {
			Func backup;
			A64HookFunction(reinterpret_cast<void*>(function), reinterpret_cast<void*>(hook), reinterpret_cast<void**>(&backup));
			return backup;
		}

		template<class SymType>
		SymType ResolveSymbol(std::string_view symbolName) {
			uintptr_t addr;

			if(Result res; R_SUCCEEDED(res = nn::ro::LookupSymbol(&addr, symbolName.data()))) {
				return reinterpret_cast<SymType>(addr);
			} else {
				bf2mods::g_Logger->LogError("[hook-ng] Failed to resolve symbol \"{}\"! (Horizon ec {})", symbolName, res);
				return reinterpret_cast<SymType>(0xDEADDEAD);
			}
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
			(void)detail::HookFunction(detail::ResolveSymbol<ReplaceHookType<>>(symbolName), &Impl::Hook);
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
			bf2mods::g_Logger->LogDebug("test: ptr is is {}", reinterpret_cast<void*>(FlattenMemberPtr(address)));
			Backup() = detail::HookFunction(reinterpret_cast<TrampolineHookType<>>(FlattenMemberPtr(address)), &Impl::Hook);
		}

		static inline void HookAt(uintptr_t address) {
			Backup() = detail::HookFunction(reinterpret_cast<TrampolineHookType<>>(address), &Impl::Hook);
		}

		static inline void HookAt(std::string_view symbolName) {
			Backup() = detail::HookFunction(detail::ResolveSymbol<TrampolineHookType<>>(symbolName), &Impl::Hook);
		}

	   private:
		static auto& Backup() {
			constinit static TrampolineHookType<> backup {};
			return backup;
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