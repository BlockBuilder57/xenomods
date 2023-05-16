// Created by block on 5/16/2023.

#pragma once

namespace mtl {

	struct ALLOC_HANDLE {
	   public:
		unsigned long actual;

		// xenomods from here
		inline bool IsValid() {
			return actual != -1ul;
		}
		inline ALLOC_HANDLE* Ptr() {
			return reinterpret_cast<ALLOC_HANDLE*>(actual);
		}
	};

}