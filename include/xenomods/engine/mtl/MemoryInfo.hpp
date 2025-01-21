#pragma once

namespace mtl {

    /// Populated by `mtl::MemManager::getMemoryInfo`
    struct MemoryInfo {
        public:
            /// Total size of the region, in bytes.
            ///
            /// For `ALLOCATOR_BF3`, this isn't always accurate because the allocator doesn't seem
            /// to actually enforce the limit, additionally it is sometimes zero for those regions.
            size_t totalSize;
            /// Bytes available for new allocations, this seems to always be
            /// `totalSize - allocatedSize`.
            ///
            /// For `ALLOCATOR_BF3`, this has the same inconsistencies as `totalSize`.
            size_t availableSize;
            /// Total bytes reserved for existing allocations, including allocation overhead.
            size_t allocatedSize;
            /// The size of the largest contiguous block available for new allocations,
            /// a low value with much available space left indicates high fragmentation.
            ///
            /// This is always zero for `ALLOCATOR_BF3`.
            size_t largestFreeSize;
            /// 
            /// Normally ranges from 0.0 to 100.0, but due to the inconsistencies explained above
            /// it could be higher for `ALLOCATOR_BF3`.
            float usedPercent;
            /// Percentage ratio of occupied chunks vs total chunks in the region.
            /// A low value indicates many short-lived allocations, high fragmentation, or both.
            ///
            /// This is always zero for `ALLOCATOR_BF3`.
            float occupiedChunksPercent;
            /// Number of chunks that are fully occupied, i.e. perfectly aligned.
            uint32_t fullyAllocatedChunks;
            /// First address in the region.
            ///
            /// This seems to be zero for `ALLOCATOR_BF3`.
            size_t topAddress;
            /// The name assigned to the memory region.
            ///
            /// As is common in that game, all names are empty in Xenoblade 3. 
            const char *regionName;

#if XENOMODS_CODENAME(bf3)
            /// Only present in Xenoblade 3
            mtl::AllocatorType allocatorType;
#endif 

    };

}