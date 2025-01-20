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
		ALLOCATOR_NULL = 0,
		ALLOCATOR_DLMALLOC = 1,
		ALLOCATOR_ASSET = 2,
		ALLOCATOR_BF3 = 3
	};

}