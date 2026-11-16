#include "figure_array.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace lab3 {

FigureArray::FigureArray(std::size_t initialCapacity)
    : figures_(initialCapacity == 0 ? nullptr : new Figure*[initialCapacity]()),
      capacity_(initialCapacity) {}

FigureArray::FigureArray(const FigureArray& other)
    : figures_(other.capacity_ == 0 ? nullptr : new Figure*[other.capacity_]()),
      size_(other.size_),
      capacity_(other.capacity_) {
    try {
        for (std::size_t i = 0; i < size_; ++i) {
            figures_[i] = other.figures_[i]->clone();
        }
    } catch (...) {
        clear();
        throw;
    }
}

FigureArray& FigureArray::operator=(const FigureArray& other) {
    if (this != &other) {
        FigureArray copy(other);
        swap(copy);
    }
    return *this;
}

FigureArray::FigureArray(FigureArray&& other) noexcept
    : figures_(std::exchange(other.figures_, nullptr)),
      size_(std::exchange(other.size_, 0)),
      capacity_(std::exchange(other.capacity_, 0)) {}

FigureArray& FigureArray::operator=(FigureArray&& other) noexcept {
    if (this != &other) {
        clear();
        figures_ = std::exchange(other.figures_, nullptr);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }
    return *this;
}

FigureArray::~FigureArray() {
    clear();
}

void FigureArray::add(Figure* figure) {
    if (figure == nullptr) {
        throw std::invalid_argument("Нельзя добавить пустой указатель");
    }
    if (size_ == capacity_) {
        grow();
    }
    figures_[size_++] = figure;
}

void FigureArray::remove(std::size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Индекс фигуры вне диапазона");
    }
    delete figures_[index];
    for (std::size_t i = index + 1; i < size_; ++i) {
        figures_[i - 1] = figures_[i];
    }
    --size_;
    figures_[size_] = nullptr;
}

Figure* FigureArray::at(std::size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Индекс фигуры вне диапазона");
    }
    return figures_[index];
}

const Figure* FigureArray::at(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Индекс фигуры вне диапазона");
    }
    return figures_[index];
}

std::size_t FigureArray::size() const noexcept {
    return size_;
}

std::size_t FigureArray::capacity() const noexcept {
    return capacity_;
}

bool FigureArray::empty() const noexcept {
    return size_ == 0;
}

double FigureArray::totalArea() const noexcept {
    double result = 0.0;
    for (std::size_t i = 0; i < size_; ++i) {
        result += static_cast<double>(*figures_[i]);
    }
    return result;
}

void FigureArray::swap(FigureArray& other) noexcept {
    std::swap(figures_, other.figures_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

void FigureArray::clear() noexcept {
    for (std::size_t i = 0; i < size_; ++i) {
        delete figures_[i];
    }
    delete[] figures_;
    figures_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

void FigureArray::grow() {
    const std::size_t newCapacity = capacity_ == 0 ? 2 : capacity_ * 2;
    Figure** newFigures = new Figure*[newCapacity]();
    for (std::size_t i = 0; i < size_; ++i) {
        newFigures[i] = figures_[i];
    }
    delete[] figures_;
    figures_ = newFigures;
    capacity_ = newCapacity;
}

}  // namespace lab3
