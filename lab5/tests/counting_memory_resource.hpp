#pragma once

#include <cstddef>
#include <memory_resource>

class CountingMemoryResource final : public std::pmr::memory_resource {
public:
    std::size_t allocations = 0;
    std::size_t deallocations = 0;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        ++allocations;
        return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    }

    void do_deallocate(void* pointer, std::size_t bytes, std::size_t alignment) override {
        ++deallocations;
        std::pmr::new_delete_resource()->deallocate(pointer, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};
