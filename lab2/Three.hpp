#pragma once

#include <cstddef>
#include <initializer_list>
#include <string>

class Three {
public:
    Three();
    Three(const std::size_t& size, unsigned char fill = 0);
    Three(const std::initializer_list<unsigned char>& values);
    Three(const std::string& value);
    Three(const Three& other);
    Three(Three&& other) noexcept;
    virtual ~Three() noexcept;

    Three add(const Three& other) const;
    Three subtract(const Three& other) const;
    Three copy() const;

    bool equals(const Three& other) const;
    bool greater(const Three& other) const;
    bool less(const Three& other) const;

    std::string toString() const;

private:
    void normalize();
    std::size_t logicalSize() const;
    unsigned char digitAt(std::size_t index) const;

    unsigned char* digits_;
    std::size_t size_;
};
