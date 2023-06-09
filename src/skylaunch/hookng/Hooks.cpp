#include <xenomods/Logger.hpp>
#include <skylaunch/hookng/Hooks.hpp>
#include <xenomods/stuff/utils/debug_util.hpp>

namespace skylaunch::hook::detail {

	void* HookFunctionBase(void* function, void* replacement) {
		void* backup;

		if (function == reinterpret_cast<void*>(INVALID_FUNCTION_PTR)) {
			return nullptr;
		}

		xenomods::g_Logger->LogDebug("[hook-ng] Attempting to hook {}...", dbgutil::getSymbol(reinterpret_cast<uintptr_t>(function)));

		A64HookFunction(function, replacement, &backup);
		return backup;
	}

	void InlineHookBase(void* addr, void* handler) {
		xenomods::g_Logger->LogDebug("[hook-ng] Inline hooking @ {}", addr);
		A64InlineHook(addr, handler);
	}

	uintptr_t ResolveSymbolBase(std::string_view symbolName) {
		uintptr_t addr;

		if(Result res; R_SUCCEEDED(res = nn::ro::LookupSymbol(&addr, symbolName.data()))) {
			return addr;
		} else {
			xenomods::g_Logger->LogError("[hook-ng] Failed to resolve symbol \"{}\"! (Horizon ec {})", symbolName, res);
			return INVALID_FUNCTION_PTR;
		}
	}

} // namespace skylaunch::hook::detail