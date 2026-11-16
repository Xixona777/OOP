#pragma once

#include <cstddef>
#include <memory_resource>
#include <vector>

namespace lab5 {

class FixedBlockMemoryResource final : public std::pmr::memory_resource {
public:
    static constexpr std::size_t max_supported_alignment = alignof(std::max_align_t);

    explicit FixedBlockMemoryResource(
        std::size_t capacity,
        std::pmr::memory_resource* upstream = std::pmr::get_default_resource());
    ~FixedBlockMemoryResource() override;

    FixedBlockMemoryResource(const FixedBlockMemoryResource&) = delete;
    FixedBlockMemoryResource& operator=(const FixedBlockMemoryResource&) = delete;
    FixedBlockMemoryResource(FixedBlockMemoryResource&&) = delete;
    FixedBlockMemoryResource& operator=(FixedBlockMemoryResource&&) = delete;

    [[nodiscard]] std::size_t occupied_block_count() const noexcept;
    [[nodiscard]] std::size_t free_block_count() const noexcept;
    [[nodiscard]] std::size_t upstream_allocation_count() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;
    [[nodiscard]] std::byte* buffer_begin() noexcept;
    [[nodiscard]] const std::byte* buffer_begin() const noexcept;
    [[nodiscard]] std::byte* buffer_end() noexcept;
    [[nodiscard]] const std::byte* buffer_end() const noexcept;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void* pointer, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

private:
    struct Block {
        std::size_t offset;
        std::size_t size;
        bool occupied;
        std::size_t alignment;
    };

    void coalesce(std::size_t index);

    std::pmr::memory_resource* upstream_;
    std::size_t capacity_;
    std::byte* buffer_ = nullptr;
    std::size_t upstream_allocation_count_ = 0;
    std::vector<Block> blocks_;
};

} // namespace lab5
