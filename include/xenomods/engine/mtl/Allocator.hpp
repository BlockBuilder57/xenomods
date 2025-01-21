// Created by block on 5/16/2023.

#pragma once

namespace mtl {

	// Unofficial name
	struct AllocHandle {
		public:
			uint32_t regionId;

			// xenomods from here
			inline bool IsValid() {
				return regionId > 0;
			}
	};

	struct ALLOC_HANDLE {
		private:
			const AllocHandle* handle;

		public:
			// xenomods
			ALLOC_HANDLE(const AllocHandle* handle) : handle(handle) {}
	};

	enum AllocatorType {
		/// Present in all games, allocator that always returns `nullptr`
		ALLOCATOR_NULL = 0,
		/// Doug Lea's memory allocator. Used extensively in DE/2, and occasionally in 3.
		/// Usually nested (smaller regions from bigger arenas)
		ALLOCATOR_DLMALLOC = 1,
		/// `AssetAllocator`, used in DE/2, unused in 3 (but still present)
		ALLOCATOR_ASSET = 2,
		/// Similar to jemalloc, maps virtual pages. Introduced in 3.
		ALLOCATOR_BF3 = 3
	};

}