#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <stdexcept>
#include <utility>

namespace lab5 {

template <class T>
class SinglyLinkedList {
private:
    struct Node {
        template <class U>
        Node(U&& initial_value, Node* next_node)
            : value(std::forward<U>(initial_value)), next(next_node) {
        }

        T value;
        Node* next;
    };

public:
    using allocator_type = std::pmr::polymorphic_allocator<T>;

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator() noexcept = default;

        reference operator*() const noexcept { return current_->value; }
        pointer operator->() const noexcept { return &current_->value; }

        Iterator& operator++() noexcept {
            current_ = current_->next;
            return *this;
        }

        Iterator operator++(int) noexcept {
            Iterator copy = *this;
            ++(*this);
            return copy;
        }

        bool operator==(const Iterator&) const noexcept = default;

    private:
        explicit Iterator(Node* current) noexcept : current_(current) {}

        Node* current_ = nullptr;

        friend class SinglyLinkedList;
        friend class ConstIterator;
    };

    class ConstIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        ConstIterator() noexcept = default;
        ConstIterator(Iterator other) noexcept : current_(other.current_) {}

        reference operator*() const noexcept { return current_->value; }
        pointer operator->() const noexcept { return &current_->value; }

        ConstIterator& operator++() noexcept {
            current_ = current_->next;
            return *this;
        }

        ConstIterator operator++(int) noexcept {
            ConstIterator copy = *this;
            ++(*this);
            return copy;
        }

        bool operator==(const ConstIterator&) const noexcept = default;

    private:
        explicit ConstIterator(const Node* current) noexcept : current_(current) {}

        const Node* current_ = nullptr;

        friend class SinglyLinkedList;
    };

    explicit SinglyLinkedList(
        std::pmr::memory_resource* resource = std::pmr::get_default_resource()) noexcept
        : allocator_(resource == nullptr ? std::pmr::get_default_resource() : resource) {
    }

    explicit SinglyLinkedList(const allocator_type& allocator) noexcept
        : allocator_(allocator) {
    }

    SinglyLinkedList(const SinglyLinkedList& other)
        : SinglyLinkedList(other, other.allocator_.resource()) {
    }

    SinglyLinkedList(const SinglyLinkedList& other, std::pmr::memory_resource* resource)
        : allocator_(resource) {
        try {
            for (const auto& value : other) {
                push_back(value);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    SinglyLinkedList(SinglyLinkedList&& other) noexcept
        : allocator_(other.allocator_),
          head_(std::exchange(other.head_, nullptr)),
          tail_(std::exchange(other.tail_, nullptr)),
          size_(std::exchange(other.size_, 0)) {
    }

    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            SinglyLinkedList copy(other, allocator_.resource());
            swap_contents(copy);
        }
        return *this;
    }

    SinglyLinkedList& operator=(SinglyLinkedList&& other) {
        if (this == &other) {
            return *this;
        }

        if (allocator_.resource() == other.allocator_.resource()) {
            clear();
            steal_contents(other);
            return *this;
        }

        SinglyLinkedList moved(allocator_.resource());
        for (auto& value : other) {
            moved.push_back(std::move(value));
        }
        swap_contents(moved);
        other.clear();
        return *this;
    }

    ~SinglyLinkedList() {
        clear();
    }

    void push_front(const T& value) {
        Node* node = create_node(value, head_);
        head_ = node;
        if (tail_ == nullptr) {
            tail_ = node;
        }
        ++size_;
    }

    void push_front(T&& value) {
        Node* node = create_node(std::move(value), head_);
        head_ = node;
        if (tail_ == nullptr) {
            tail_ = node;
        }
        ++size_;
    }

    void push_back(const T& value) {
        Node* node = create_node(value, nullptr);
        append_node(node);
    }

    void push_back(T&& value) {
        Node* node = create_node(std::move(value), nullptr);
        append_node(node);
    }

    bool pop_front() noexcept {
        if (head_ == nullptr) {
            return false;
        }

        Node* old_head = head_;
        head_ = head_->next;
        if (head_ == nullptr) {
            tail_ = nullptr;
        }
        destroy_node(old_head);
        --size_;
        return true;
    }

    void clear() noexcept {
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->next;
            destroy_node(current);
            current = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    [[nodiscard]] bool empty() const noexcept {
        return head_ == nullptr;
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return size_;
    }

    T& front() {
        if (head_ == nullptr) {
            throw std::out_of_range("front() called on an empty list");
        }
        return head_->value;
    }

    const T& front() const {
        if (head_ == nullptr) {
            throw std::out_of_range("front() called on an empty list");
        }
        return head_->value;
    }

    Iterator begin() noexcept { return Iterator(head_); }
    Iterator end() noexcept { return Iterator(); }
    ConstIterator begin() const noexcept { return ConstIterator(head_); }
    ConstIterator end() const noexcept { return ConstIterator(); }
    ConstIterator cbegin() const noexcept { return ConstIterator(head_); }
    ConstIterator cend() const noexcept { return ConstIterator(); }

    [[nodiscard]] allocator_type get_allocator() const noexcept {
        return allocator_type(allocator_.resource());
    }

private:
    using node_allocator = typename std::allocator_traits<allocator_type>::template rebind_alloc<Node>;
    using node_traits = std::allocator_traits<node_allocator>;

    template <class U>
    Node* create_node(U&& value, Node* next) {
        Node* node = node_traits::allocate(allocator_, 1);
        try {
            node_traits::construct(
                allocator_, node, std::forward<U>(value), next);
        } catch (...) {
            node_traits::deallocate(allocator_, node, 1);
            throw;
        }
        return node;
    }

    void append_node(Node* node) noexcept {
        if (tail_ == nullptr) {
            head_ = node;
            tail_ = node;
        } else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    void destroy_node(Node* node) noexcept {
        node_traits::destroy(allocator_, node);
        node_traits::deallocate(allocator_, node, 1);
    }

    void steal_contents(SinglyLinkedList& other) noexcept {
        head_ = std::exchange(other.head_, nullptr);
        tail_ = std::exchange(other.tail_, nullptr);
        size_ = std::exchange(other.size_, 0);
    }

    void swap_contents(SinglyLinkedList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }

    node_allocator allocator_;
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    std::size_t size_ = 0;
};

static_assert(std::forward_iterator<typename SinglyLinkedList<int>::Iterator>);
static_assert(std::forward_iterator<typename SinglyLinkedList<int>::ConstIterator>);

} // namespace lab5
