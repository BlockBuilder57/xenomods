#pragma once

#include "Allocator.hpp"
#include "MemoryInfo.hpp"

namespace mtl {

    class MemManager {
        public:
            static bool getMemoryInfo(mtl::ALLOC_HANDLE, mtl::MemoryInfo *);
    };

}