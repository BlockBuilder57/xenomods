#include <bf2mods/stuff/utils/util.hpp>

namespace util::detail {
	bool ResolveBase(void** Output, const char* name) {
		uintptr_t addr;

		if(R_SUCCEEDED(nn::ro::LookupSymbol(&addr, name))) {
			LOG("[bf2mods.ResolveBase] Resolved \"{}\" successfully to {}",
				name, reinterpret_cast<void*>(addr));
			*Output = reinterpret_cast<void*>(addr);
			return true;
		} else {
			skylaunch::logger::s_Instance->LogFormat("[bf2mods.ResolveBase] Failed to look \"{}\" up!", name);
			// Set the output pointer to a obviously invalid value,
			// so that in errors (if this ends up getting called/used)
			// the error will show 0xDEADDEAD
			*Output = reinterpret_cast<void*>(0xDEADDEAD);
		}
		return false;
	}

	void nnRandBase(void* input, std::size_t size) {
		nn::os::GenerateRandomBytes(input, size);
	}

} // namespace util::detail
