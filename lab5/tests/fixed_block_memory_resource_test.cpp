#include "counting_memory_resource.hpp"
#include "fixed_block_memory_resource.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <new>

namespace {

TEST(FixedBlockMemoryResourceTest, AllocationsStayInsideSingleFixedBuffer) {
    CountingMemoryResource upstream;
    lab5::FixedBlockMemoryResource resource(512, &upstream);

    void* first = resource.allocate(24, 8);
    void* second = resource.allocate(80, 16);

    const auto begin = reinterpret_cast<std::uintptr_t>(resource.buffer_begin());
    const auto end = reinterpret_cast<std::uintptr_t>(resource.buffer_end());
    const auto first_address = reinterpret_cast<std::uintptr_t>(first);
    const auto second_address = reinterpret_cast<std::uintptr_t>(second);

    EXPECT_GE(first_address, begin);
    EXPECT_LE(first_address + 24, end);
    EXPECT_GE(second_address, begin);
    EXPECT_LE(second_address + 80, end);
    EXPECT_EQ(upstream.allocations, 1);
    EXPECT_EQ(resource.upstream_allocation_count(), 1);

    resource.deallocate(first, 24, 8);
    resource.deallocate(second, 80, 16);
}

TEST(FixedBlockMemoryResourceTest, ReusesReleasedBlockAndSplitsIt) {
    lab5::FixedBlockMemoryResource resource(256);
    void* original = resource.allocate(64, 8);
    resource.deallocate(original, 64, 8);

    void* smaller = resource.allocate(16, 8);

    EXPECT_EQ(smaller, original);
    EXPECT_EQ(resource.occupied_block_count(), 1);
    EXPECT_EQ(resource.free_block_count(), 1);
    EXPECT_EQ(resource.upstream_allocation_count(), 1);
    resource.deallocate(smaller, 16, 8);
}

TEST(FixedBlockMemoryResourceTest, CoalescesAdjacentFreeBlocks) {
    lab5::FixedBlockMemoryResource resource(256);
    void* first = resource.allocate(32, 8);
    void* second = resource.allocate(32, 8);
    void* third = resource.allocate(32, 8);

    resource.deallocate(first, 32, 8);
    resource.deallocate(second, 32, 8);

    void* combined = resource.allocate(64, 8);
    EXPECT_EQ(combined, first);
    EXPECT_EQ(resource.occupied_block_count(), 2);

    resource.deallocate(combined, 64, 8);
    resource.deallocate(third, 32, 8);
    EXPECT_EQ(resource.occupied_block_count(), 0);
    EXPECT_EQ(resource.free_block_count(), 1);
}

TEST(FixedBlockMemoryResourceTest, AlignsAllocationsAndRejectsUnsupportedAlignment) {
    lab5::FixedBlockMemoryResource resource(256);
    void* first = resource.allocate(3, 1);
    void* aligned = resource.allocate(7, 16);

    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(aligned) % 16, 0);
    EXPECT_THROW(static_cast<void>(resource.allocate(8, 3)), std::bad_alloc);
    EXPECT_THROW(
        static_cast<void>(resource.allocate(
            8, lab5::FixedBlockMemoryResource::max_supported_alignment * 2)),
        std::bad_alloc);

    resource.deallocate(first, 3, 1);
    resource.deallocate(aligned, 7, 16);
}

TEST(FixedBlockMemoryResourceTest, ThrowsBadAllocWhenBufferIsExhausted) {
    lab5::FixedBlockMemoryResource resource(64);
    void* whole_buffer = resource.allocate(64, 1);

    EXPECT_THROW(static_cast<void>(resource.allocate(1, 1)), std::bad_alloc);
    EXPECT_EQ(resource.occupied_block_count(), 1);
    resource.deallocate(whole_buffer, 64, 1);
}

TEST(FixedBlockMemoryResourceTest, DestructorReturnsBufferWithLiveAllocations) {
    CountingMemoryResource upstream;
    {
        lab5::FixedBlockMemoryResource resource(128, &upstream);
        static_cast<void>(resource.allocate(32, 8));
        static_cast<void>(resource.allocate(32, 8));
        EXPECT_EQ(resource.occupied_block_count(), 2);
        EXPECT_EQ(upstream.deallocations, 0);
    }
    EXPECT_EQ(upstream.allocations, 1);
    EXPECT_EQ(upstream.deallocations, 1);
}

TEST(FixedBlockMemoryResourceTest, ResourceEqualityUsesIdentity) {
    lab5::FixedBlockMemoryResource first(64);
    lab5::FixedBlockMemoryResource second(64);

    EXPECT_TRUE(first.is_equal(first));
    EXPECT_FALSE(first.is_equal(second));
}

} // namespace
