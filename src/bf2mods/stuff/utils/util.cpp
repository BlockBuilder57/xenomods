#include <bf2mods/stuff/utils/util.hpp>

namespace util {

	namespace detail {
		bool ResolveBase(void** Output, const char* name) {
			uintptr_t addr;

			if (R_SUCCEEDED(nn::ro::LookupSymbol(&addr, name))) {
				skyline::logger::s_Instance->LogFormat("[bf2mods.ResolveBase] Resolved \"%s\" successfully to %p\n",
				                                       name, addr);
				*Output = reinterpret_cast<void*>(addr);
				return true;
			} else {
				skyline::logger::s_Instance->LogFormat("[bf2mods.ResolveBase] Failed to look \"%s\" up!\n", name);
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
	}

	FpsLogger::FpsLogger()
			: m_lastSecondTick(armGetSystemTick()) {
	}

	void FpsLogger::tick() {
		auto curTick = armGetSystemTick();
		if (curTick - m_lastSecondTick > armGetSystemTickFreq()) {
			LOG("%d fps", m_frameCount);
			m_frameCount = 0;
			m_lastSecondTick = armGetSystemTick();
		} else {
			m_frameCount++;
		}
	}

}

