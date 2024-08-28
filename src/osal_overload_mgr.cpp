#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <iostream>

#include "osal_debug.h"

// 最小分配块大小
/*
 * 跑完整的osal_test, header为4 bytes,测试结果：
 * 1   : 92.5% Max:1744 bytes
 * 16  : 82.0% Max:1832 bytes
 * 32  : 71.0% Max:1944 bytes
 * 48  : 57.2% Max:2056 bytes
 * 64  : 57.0% Max:2008 bytes
 * 128 : 33.7% Max:1832 bytes
 */
const std::size_t MIN_BLOCK_SIZE = 16;

// 对齐到最小块大小的倍数
std::size_t alignSize(std::size_t size) { return (size + MIN_BLOCK_SIZE - 1) & ~(MIN_BLOCK_SIZE - 1); }

// 统计数据
struct MemoryStats {
    std::atomic<std::size_t> totalAllocated{0};
    std::atomic<std::size_t> totalFreed{0};
    std::atomic<std::size_t> currentUsage{0};
    std::atomic<std::size_t> allocationCount{0};
    std::atomic<std::size_t> freeCount{0};
    std::atomic<std::size_t> maxAllocated{0};
    std::atomic<std::size_t> extraAllocationDueToAlignment{0};  // 由于对齐导致的额外分配
    std::atomic<std::size_t> extraAllocationDueToHeader{0};     // 由于BlockHeader导致的额外分配
    std::atomic<bool> memoryLow{false};                         // 内存不足标志
};

static MemoryStats memStats;

// 自定义内存块头部，用于存储块大小
struct BlockHeader {
    std::size_t size;
};

// 计算内存有效利用率
static float calculateMemoryUtilization(const MemoryStats& stats) {
    std::size_t totalAllocated = stats.totalAllocated.load(std::memory_order_relaxed);
    std::size_t extraAllocationDueToAlignment = stats.extraAllocationDueToAlignment.load(std::memory_order_relaxed);
    std::size_t extraAllocationDueToHeader = stats.extraAllocationDueToHeader.load(std::memory_order_relaxed);
    std::size_t totalExtraAllocation = extraAllocationDueToAlignment + extraAllocationDueToHeader;
    if (totalAllocated == 0) return 0.0f;
    float result = static_cast<float>(totalAllocated - totalExtraAllocation) / totalAllocated;
    return result;
}

// 自定义 malloc
void* dp_malloc(std::size_t size) {
    std::size_t totalSize = alignSize(size + sizeof(BlockHeader));
    BlockHeader* header = static_cast<BlockHeader*>(std::malloc(totalSize));
    if (!header) {
        osal::OSAL_LOGE("Memory allocation failed for size: %d\n", static_cast<int>(totalSize));
        memStats.memoryLow.store(true, std::memory_order_relaxed);  // 设置内存不足标志
        return nullptr;
    }
    header->size = size;
    void* ptr = static_cast<void*>(header + 1);
    std::size_t alignedSize = totalSize - sizeof(BlockHeader);
    memStats.totalAllocated.fetch_add(alignedSize, std::memory_order_relaxed);
    memStats.currentUsage.fetch_add(alignedSize, std::memory_order_relaxed);
    memStats.allocationCount.fetch_add(1, std::memory_order_relaxed);
    memStats.extraAllocationDueToAlignment.fetch_add(alignedSize - size, std::memory_order_relaxed);
    memStats.extraAllocationDueToHeader.fetch_add(sizeof(BlockHeader), std::memory_order_relaxed);

    // 更新最大分配内存大小
    std::size_t currentUsage = memStats.currentUsage.load(std::memory_order_relaxed);
    std::size_t previousMaxAllocated = memStats.maxAllocated.load(std::memory_order_relaxed);
    while (currentUsage > previousMaxAllocated && !memStats.maxAllocated.compare_exchange_weak(
                                                      previousMaxAllocated, currentUsage, std::memory_order_relaxed)) {
        // 继续尝试更新，直到成功
    }

    osal::OSAL_LOGD("Allocated %d bytes, total allocated: %d, current usage: %d\n", static_cast<int>(alignedSize),
                    static_cast<int>(memStats.totalAllocated.load(std::memory_order_relaxed)),
                    static_cast<int>(memStats.currentUsage.load(std::memory_order_relaxed)));
    return ptr;
}

// 自定义 free
void dp_free(void* ptr) {
    if (!ptr) return;
    BlockHeader* header = static_cast<BlockHeader*>(ptr) - 1;
    std::size_t size = header->size;
    std::size_t alignedSize = alignSize(size + sizeof(BlockHeader)) - sizeof(BlockHeader);
    std::free(header);
    memStats.totalFreed.fetch_add(alignedSize, std::memory_order_relaxed);
    memStats.currentUsage.fetch_sub(alignedSize, std::memory_order_relaxed);
    memStats.freeCount.fetch_add(1, std::memory_order_relaxed);
    osal::OSAL_LOGD("Freed %d bytes, total freed: %d, current usage: %d\n", static_cast<int>(alignedSize),
                    static_cast<int>(memStats.totalFreed.load(std::memory_order_relaxed)),
                    static_cast<int>(memStats.currentUsage.load(std::memory_order_relaxed)));
}

// 自定义 new 运算符
void* operator new(std::size_t size) { return dp_malloc(size); }

// 自定义 delete 运算符
void operator delete(void* ptr) noexcept { dp_free(ptr); }
void operator delete(void* ptr, std::size_t) noexcept { dp_free(ptr); }

// 自定义 new[] 运算符
void* operator new[](std::size_t size) { return dp_malloc(size); }

// 自定义 delete[] 运算符
void operator delete[](void* ptr) noexcept { dp_free(ptr); }
void operator delete[](void* ptr, std::size_t) noexcept { dp_free(ptr); }

void MemoryStatsPrint(void) {
    // 打印统计数据
    osal::OSAL_LOGI("Header Size %d bytes, MIN_BLOCK_SIZE: %d bytes\n", sizeof(BlockHeader), MIN_BLOCK_SIZE);
    osal::OSAL_LOGI("Total allocated: %d bytes\n",
                    static_cast<int>(memStats.totalAllocated.load(std::memory_order_relaxed)));
    osal::OSAL_LOGI("Total freed: %d bytes\n", static_cast<int>(memStats.totalFreed.load(std::memory_order_relaxed)));
    osal::OSAL_LOGI("Current usage: %d bytes\n",
                    static_cast<int>(memStats.currentUsage.load(std::memory_order_relaxed)));
    osal::OSAL_LOGI("Allocation count: %d\n",
                    static_cast<int>(memStats.allocationCount.load(std::memory_order_relaxed)));
    osal::OSAL_LOGI("Free count: %d\n", static_cast<int>(memStats.freeCount.load(std::memory_order_relaxed)));
    osal::OSAL_LOGI("Max usage :%d bytes\n", static_cast<int>(memStats.maxAllocated.load(std::memory_order_relaxed)));
    osal::OSAL_LOGI("Memory utilization: %f\n", calculateMemoryUtilization(memStats));
    if (memStats.memoryLow.load(std::memory_order_relaxed)) {
        osal::OSAL_LOGE("Memory is low!\n");
    }
}
