#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

template<class T>
class Array {
public:
    Array() = default;

    explicit Array(std::size_t capacity)
        : data_(capacity == 0 ? nullptr : std::make_shared<T[]>(capacity)),
          capacity_(capacity) {}

    Array(const Array& other)
        : data_(other.capacity_ == 0 ? nullptr
                                    : std::make_shared<T[]>(other.capacity_)),
          size_(other.size_),
          capacity_(other.capacity_) {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    Array(Array&& other) noexcept
        : data_(std::move(other.data_)),
          size_(other.size_),
          capacity_(other.capacity_) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    Array& operator=(const Array& other) {
        if (this != &other) {
            Array copy(other);
            swap(copy);
        }
        return *this;
    }

    Array& operator=(Array&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    ~Array() = default;

    void push_back(const T& value) {
        ensureCapacityForOneMore();
        data_[size_++] = value;
    }

    void push_back(T&& value) {
        ensureCapacityForOneMore();
        data_[size_++] = std::move(value);
    }

    void remove(std::size_t index) {
        checkIndex(index);
        for (std::size_t i = index; i + 1 < size_; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        --size_;
        data_[size_] = T{};
    }

    T& operator[](std::size_t index) {
        checkIndex(index);
        return data_[index];
    }

    const T& operator[](std::size_t index) const {
        checkIndex(index);
        return data_[index];
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return size_;
    }

    [[nodiscard]] std::size_t capacity() const noexcept {
        return capacity_;
    }

    void swap(Array& other) noexcept {
        data_.swap(other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    void ensureCapacityForOneMore() {
        if (size_ < capacity_) {
            return;
        }

        const std::size_t newCapacity = capacity_ == 0 ? 1 : capacity_ * 2;
        auto newData = std::make_shared<T[]>(newCapacity);
        for (std::size_t i = 0; i < size_; ++i) {
            newData[i] = std::move(data_[i]);
        }
        data_ = std::move(newData);
        capacity_ = newCapacity;
    }

    void checkIndex(std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Array index is out of range");
        }
    }

    std::shared_ptr<T[]> data_{};
    std::size_t size_{0};
    std::size_t capacity_{0};
};
