#pragma once

#include "Allocator.hpp"
#include "MemoryInfo.hpp"
#include "xenomods/Version.hpp"

namespace mtl {

	class MemManager {
	   public:
		static bool getMemoryInfo(mtl::ALLOC_HANDLE, mtl::MemoryInfo*);

		// xenomods
		/// `mtl::MemoryInfo` accessor compatible with all games
		static bool GET_MEMORY_INFO(mtl::ALLOC_HANDLE handle, mtl::MemoryInfo* out) {
#if !XENOMODS_CODENAME(bf3)
			return getMemoryInfo(handle, out);
#else
			uintptr_t getMemoryInfo;
			if(xenomods::version::RuntimeVersion() == xenomods::version::SemVer::v2_0_0)
				getMemoryInfo = skylaunch::utils::AddrFromBase(0x710128a670);
			else if(xenomods::version::RuntimeVersion() == xenomods::version::SemVer::v2_1_0)
				getMemoryInfo = skylaunch::utils::AddrFromBase(0x710128a9a0);
			else if(xenomods::version::RuntimeVersion() == xenomods::version::SemVer::v2_1_1)
				getMemoryInfo = skylaunch::utils::AddrFromBase(0x710128a9e0);
			else if(xenomods::version::RuntimeVersion() == xenomods::version::SemVer::v2_2_0)
				getMemoryInfo = skylaunch::utils::AddrFromBase(0x710128b550);

			return ((bool (*)(mtl::ALLOC_HANDLE, mtl::MemoryInfo*))getMemoryInfo)(handle, out);
#endif
		}
	};

} // namespace mtl