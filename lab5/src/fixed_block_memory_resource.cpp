#include "fixed_block_memory_resource.hpp"

#include <algorithm>
#include <cstdint>
#include <new>
#include <stdexcept>

namespace lab5 {

FixedBlockMemoryResource::FixedBlockMemoryResource(
    std::size_t capacity,
    std::pmr::memory_resource* upstream)
    : upstream_(upstream), capacity_(capacity) {
    if (upstream_ == nullptr) {
        throw std::invalid_argument("upstream memory_resource must not be null");
    }
    if (capacity_ == 0) {
        throw std::invalid_argument("fixed buffer capacity must be positive");
    }

    buffer_ = static_cast<std::byte*>(
        upstream_->allocate(capacity_, max_supported_alignment));
    ++upstream_allocation_count_;

    try {
        blocks_.push_back(Block{0, capacity_, false, max_supported_alignment});
    } catch (...) {
        upstream_->deallocate(buffer_, capacity_, max_supported_alignment);
        buffer_ = nullptr;
        throw;
    }
}

FixedBlockMemoryResource::~FixedBlockMemoryResource() {
    upstream_->deallocate(buffer_, capacity_, max_supported_alignment);
}

std::size_t FixedBlockMemoryResource::occupied_block_count() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        blocks_.begin(), blocks_.end(), [](const Block& block) { return block.occupied; }));
}

std::size_t FixedBlockMemoryResource::free_block_count() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        blocks_.begin(), blocks_.end(), [](const Block& block) { return !block.occupied; }));
}

std::size_t FixedBlockMemoryResource::upstream_allocation_count() const noexcept {
    return upstream_allocation_count_;
}

std::size_t FixedBlockMemoryResource::capacity() const noexcept {
    return capacity_;
}

std::byte* FixedBlockMemoryResource::buffer_begin() noexcept {
    return buffer_;
}

const std::byte* FixedBlockMemoryResource::buffer_begin() const noexcept {
    return buffer_;
}

std::byte* FixedBlockMemoryResource::buffer_end() noexcept {
    return buffer_ + capacity_;
}

const std::byte* FixedBlockMemoryResource::buffer_end() const noexcept {
    return buffer_ + capacity_;
}

void* FixedBlockMemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
    if (alignment == 0 || (alignment & (alignment - 1)) != 0 ||
        alignment > max_supported_alignment) {
        throw std::bad_alloc();
    }
    if (bytes == 0) {
        bytes = 1;
    }

    for (std::size_t index = 0; index < blocks_.size(); ++index) {
        const Block candidate = blocks_[index];
        if (candidate.occupied) {
            continue;
        }

        const std::size_t remainder = candidate.offset % alignment;
        const std::size_t padding = remainder == 0 ? 0 : alignment - remainder;
        if (padding > candidate.size || bytes > candidate.size - padding) {
            continue;
        }

        const std::size_t aligned_offset = candidate.offset + padding;
        const std::size_t suffix_size = candidate.size - padding - bytes;
        blocks_.reserve(blocks_.size() + 2);

        blocks_[index] = Block{aligned_offset, bytes, true, alignment};
        if (suffix_size != 0) {
            blocks_.insert(
                blocks_.begin() + static_cast<std::ptrdiff_t>(index + 1),
                Block{aligned_offset + bytes, suffix_size, false, max_supported_alignment});
        }
        if (padding != 0) {
            blocks_.insert(
                blocks_.begin() + static_cast<std::ptrdiff_t>(index),
                Block{candidate.offset, padding, false, max_supported_alignment});
        }

        return buffer_ + aligned_offset;
    }

    throw std::bad_alloc();
}

void FixedBlockMemoryResource::do_deallocate(
    void* pointer,
    std::size_t /*bytes*/,
    std::size_t /*alignment*/) {
    if (pointer == nullptr) {
        return;
    }

    const auto address = reinterpret_cast<std::uintptr_t>(pointer);
    const auto begin = reinterpret_cast<std::uintptr_t>(buffer_);
    const auto end = begin + capacity_;
    if (address < begin || address >= end) {
        return;
    }

    const auto offset = static_cast<std::size_t>(address - begin);
    for (std::size_t index = 0; index < blocks_.size(); ++index) {
        if (blocks_[index].occupied && blocks_[index].offset == offset) {
            blocks_[index].occupied = false;
            coalesce(index);
            return;
        }
    }
}

bool FixedBlockMemoryResource::do_is_equal(
    const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}

void FixedBlockMemoryResource::coalesce(std::size_t index) {
    if (index + 1 < blocks_.size() && !blocks_[index + 1].occupied) {
        blocks_[index].size += blocks_[index + 1].size;
        blocks_.erase(blocks_.begin() + static_cast<std::ptrdiff_t>(index + 1));
    }
    if (index > 0 && !blocks_[index - 1].occupied) {
        blocks_[index - 1].size += blocks_[index].size;
        blocks_.erase(blocks_.begin() + static_cast<std::ptrdiff_t>(index));
    }
}

} // namespace lab5
