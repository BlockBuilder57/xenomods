#pragma once

namespace mtl {

    struct MemoryInfo {
        public:
            size_t totalSize;
            size_t availableSize;
            size_t allocatedSize;
            size_t largestFreeSize;
            float usedPercent;
            float allocatedChunksPercent;
            uint32_t fullyAllocatedChunks;
            size_t topAddress;
            const char *regionName;

#if XENOMODS_CODENAME(bf3)
            mtl::AllocatorType allocatorType;
#endif 

    };

}