#pragma once

#include "figure.hpp"

#include <cstddef>

namespace lab3 {

class FigureArray {
public:
    FigureArray() = default;
    explicit FigureArray(std::size_t initialCapacity);
    FigureArray(const FigureArray& other);
    FigureArray& operator=(const FigureArray& other);
    FigureArray(FigureArray&& other) noexcept;
    FigureArray& operator=(FigureArray&& other) noexcept;
    ~FigureArray();

    void add(Figure* figure);
    void remove(std::size_t index);
    Figure* at(std::size_t index);
    const Figure* at(std::size_t index) const;
    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;
    bool empty() const noexcept;
    double totalArea() const noexcept;

    void swap(FigureArray& other) noexcept;

private:
    void clear() noexcept;
    void grow();

    Figure** figures_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};

}  // namespace lab3
