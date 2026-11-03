#include "Three.hpp"

#include <algorithm>
#include <stdexcept>

Three::Three() : Three(1, 0) {}

Three::Three(const std::size_t& size, unsigned char fill) : digits_(nullptr), size_(0) {
    if (size == 0) {
        throw std::invalid_argument("Three size must be greater than zero");
    }
    if (fill > 2) {
        throw std::invalid_argument("Three digit must be 0, 1 or 2");
    }

    digits_ = new unsigned char[size];
    size_ = size;
    std::fill(digits_, digits_ + size_, fill);
    normalize();
}

Three::Three(const std::initializer_list<unsigned char>& values)
    : digits_(nullptr), size_(0) {
    for (unsigned char digit : values) {
        if (digit > 2) {
            throw std::invalid_argument("Three digit must be 0, 1 or 2");
        }
    }

    size_ = values.size() == 0 ? 1 : values.size();
    digits_ = new unsigned char[size_];

    if (values.size() == 0) {
        digits_[0] = 0;
        return;
    }

    std::size_t index = size_;
    for (unsigned char digit : values) {
        digits_[--index] = digit;
    }
    normalize();
}

Three::Three(const std::string& value) : digits_(nullptr), size_(0) {
    if (value.empty()) {
        throw std::invalid_argument("Three string must not be empty");
    }
    for (char digit : value) {
        if (digit < '0' || digit > '2') {
            throw std::invalid_argument("Three string must contain only digits 0, 1 and 2");
        }
    }

    size_ = value.size();
    digits_ = new unsigned char[size_];
    for (std::size_t i = 0; i < size_; ++i) {
        digits_[i] = static_cast<unsigned char>(value[size_ - 1 - i] - '0');
    }
    normalize();
}

Three::Three(const Three& other)
    : digits_(other.digits_ == nullptr ? nullptr : new unsigned char[other.size_]),
      size_(other.size_) {
    if (digits_ != nullptr) {
        std::copy(other.digits_, other.digits_ + size_, digits_);
    }
}

Three::Three(Three&& other) noexcept : digits_(other.digits_), size_(other.size_) {
    other.digits_ = nullptr;
    other.size_ = 0;
}

Three::~Three() noexcept {
    delete[] digits_;
}

Three Three::add(const Three& other) const {
    const std::size_t maxSize = std::max(logicalSize(), other.logicalSize());
    Three result(maxSize + 1, 0);
    result.size_ = maxSize + 1;
    unsigned char carry = 0;

    for (std::size_t i = 0; i < maxSize; ++i) {
        const unsigned char sum = digitAt(i) + other.digitAt(i) + carry;
        result.digits_[i] = sum % 3;
        carry = sum / 3;
    }
    result.digits_[maxSize] = carry;
    result.normalize();
    return result;
}

Three Three::subtract(const Three& other) const {
    if (less(other)) {
        throw std::underflow_error("Three subtraction result cannot be negative");
    }

    const std::size_t resultSize = logicalSize();
    Three result(resultSize, 0);
    result.size_ = resultSize;
    int borrow = 0;
    for (std::size_t i = 0; i < resultSize; ++i) {
        int difference = static_cast<int>(digitAt(i)) - other.digitAt(i) - borrow;
        if (difference < 0) {
            difference += 3;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.digits_[i] = static_cast<unsigned char>(difference);
    }
    result.normalize();
    return result;
}

Three Three::copy() const {
    return Three(*this);
}

bool Three::equals(const Three& other) const {
    if (logicalSize() != other.logicalSize()) {
        return false;
    }
    for (std::size_t i = 0; i < logicalSize(); ++i) {
        if (digitAt(i) != other.digitAt(i)) {
            return false;
        }
    }
    return true;
}

bool Three::greater(const Three& other) const {
    if (logicalSize() != other.logicalSize()) {
        return logicalSize() > other.logicalSize();
    }
    for (std::size_t i = logicalSize(); i > 0; --i) {
        if (digitAt(i - 1) != other.digitAt(i - 1)) {
            return digitAt(i - 1) > other.digitAt(i - 1);
        }
    }
    return false;
}

bool Three::less(const Three& other) const {
    return !equals(other) && !greater(other);
}

std::string Three::toString() const {
    std::string result(logicalSize(), '0');
    for (std::size_t i = 0; i < logicalSize(); ++i) {
        result[logicalSize() - 1 - i] = static_cast<char>('0' + digitAt(i));
    }
    return result;
}

void Three::normalize() {
    while (size_ > 1 && digits_[size_ - 1] == 0) {
        --size_;
    }
}

std::size_t Three::logicalSize() const {
    return size_ == 0 ? 1 : size_;
}

unsigned char Three::digitAt(std::size_t index) const {
    return index < size_ ? digits_[index] : 0;
}
