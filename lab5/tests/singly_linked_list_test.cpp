#include "fixed_block_memory_resource.hpp"
#include "record.hpp"
#include "singly_linked_list.hpp"

#include <gtest/gtest.h>

#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

class SinglyLinkedListTest : public testing::Test {
protected:
    lab5::FixedBlockMemoryResource resource{16 * 1024};
};

TEST_F(SinglyLinkedListTest, IntOperationsUseFixedResource) {
    lab5::SinglyLinkedList<int> list(&resource);
    EXPECT_TRUE(list.empty());
    EXPECT_FALSE(list.pop_front());
    EXPECT_THROW(static_cast<void>(list.front()), std::out_of_range);

    list.push_back(2);
    list.push_front(1);
    list.push_back(3);

    ASSERT_EQ(list.size(), 3);
    EXPECT_EQ(list.front(), 1);
    EXPECT_EQ(resource.occupied_block_count(), 3);
    EXPECT_EQ(list.get_allocator().resource(), &resource);

    EXPECT_TRUE(list.pop_front());
    EXPECT_EQ(list.front(), 2);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(resource.occupied_block_count(), 2);
}

TEST_F(SinglyLinkedListTest, RecordOperationsPreserveValues) {
    lab5::SinglyLinkedList<lab5::Record> records(&resource);
    records.push_back(lab5::Record{2, "Борис", 80.5});
    records.push_front(lab5::Record{1, "Анна", 95.0});

    ASSERT_EQ(records.size(), 2);
    EXPECT_EQ(records.front(), (lab5::Record{1, "Анна", 95.0}));
    records.front().score = 97.5;
    EXPECT_DOUBLE_EQ(records.front().score, 97.5);

    EXPECT_TRUE(records.pop_front());
    EXPECT_EQ(records.front().name, "Борис");
    records.clear();
    EXPECT_TRUE(records.empty());
    EXPECT_EQ(resource.occupied_block_count(), 0);
}

TEST_F(SinglyLinkedListTest, ClearAllowsRepeatedRefillingAndMemoryReuse) {
    lab5::SinglyLinkedList<int> list(&resource);
    for (int value = 0; value < 20; ++value) {
        list.push_back(value);
    }
    const int* first_value_address = &list.front();
    list.clear();
    EXPECT_EQ(resource.free_block_count(), 1);

    list.push_front(100);
    EXPECT_EQ(&list.front(), first_value_address);
    for (int value = 101; value < 120; ++value) {
        list.push_front(value);
    }
    EXPECT_EQ(list.size(), 20);
    EXPECT_EQ(resource.occupied_block_count(), 20);

    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(list.begin(), list.end());
    EXPECT_EQ(resource.occupied_block_count(), 0);
}

TEST_F(SinglyLinkedListTest, ForwardAndConstIteratorsSupportStandardAlgorithms) {
    lab5::SinglyLinkedList<int> list(&resource);
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    EXPECT_EQ(std::distance(list.begin(), list.end()), 3);
    EXPECT_EQ(std::distance(list.cbegin(), list.cend()), 3);

    auto first = list.begin();
    auto second = first;
    ++first;
    EXPECT_EQ(*first, 2);
    EXPECT_EQ(*second, 1);
    auto old = second++;
    EXPECT_EQ(*old, 1);
    EXPECT_EQ(*second, 2);

    *first = 20;
    const auto& const_list = list;
    EXPECT_EQ(*const_list.begin(), 1);
    EXPECT_EQ(*std::next(const_list.begin()), 20);
    EXPECT_TRUE(const_list.begin() == list.begin());

    std::vector<int> visited;
    for (int value : const_list) {
        visited.push_back(value);
    }
    EXPECT_EQ(visited, (std::vector<int>{1, 20, 3}));
}

TEST_F(SinglyLinkedListTest, CopyingIsDeepAndAssignmentKeepsDestinationResource) {
    lab5::SinglyLinkedList<int> original(&resource);
    original.push_back(1);
    original.push_back(2);

    lab5::SinglyLinkedList<int> copy(original);
    copy.front() = 10;
    copy.push_back(3);
    EXPECT_EQ(original.front(), 1);
    EXPECT_EQ(original.size(), 2);
    EXPECT_EQ(copy.size(), 3);

    lab5::FixedBlockMemoryResource other_resource(1024);
    lab5::SinglyLinkedList<int> assigned(&other_resource);
    assigned = original;
    EXPECT_EQ(assigned.get_allocator().resource(), &other_resource);
    EXPECT_EQ(
        std::vector<int>(assigned.begin(), assigned.end()),
        std::vector<int>(original.begin(), original.end()));

    assigned.pop_front();
    EXPECT_EQ(original.front(), 1);
    EXPECT_EQ(assigned.front(), 2);
}

TEST_F(SinglyLinkedListTest, MoveHandlesSameAndDifferentResources) {
    lab5::SinglyLinkedList<int> source(&resource);
    source.push_back(1);
    source.push_back(2);
    lab5::SinglyLinkedList<int> moved(std::move(source));
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(moved.size(), 2);

    lab5::FixedBlockMemoryResource other_resource(1024);
    lab5::SinglyLinkedList<int> destination(&other_resource);
    destination = std::move(moved);
    EXPECT_TRUE(moved.empty());
    EXPECT_EQ(destination.get_allocator().resource(), &other_resource);
    EXPECT_EQ(destination.front(), 1);
    EXPECT_EQ(std::distance(destination.begin(), destination.end()), 2);
    EXPECT_EQ(resource.occupied_block_count(), 0);
}

struct ThrowingValue {
    explicit ThrowingValue(int initial) : value(initial) {}

    ThrowingValue(const ThrowingValue& other) : value(other.value) {
        if (throw_on_copy) {
            throw std::runtime_error("copy failed");
        }
    }

    ThrowingValue(ThrowingValue&&) = default;
    ThrowingValue& operator=(const ThrowingValue&) = default;
    ThrowingValue& operator=(ThrowingValue&&) = default;

    int value;
    static inline bool throw_on_copy = false;
};

TEST_F(SinglyLinkedListTest, ConstructorExceptionDoesNotLeakNode) {
    lab5::SinglyLinkedList<ThrowingValue> list(&resource);
    ThrowingValue value(42);
    ThrowingValue::throw_on_copy = true;
    EXPECT_THROW(list.push_front(value), std::runtime_error);
    ThrowingValue::throw_on_copy = false;

    EXPECT_TRUE(list.empty());
    EXPECT_EQ(resource.occupied_block_count(), 0);
    EXPECT_EQ(resource.free_block_count(), 1);
}

} // namespace
