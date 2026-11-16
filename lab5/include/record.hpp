#pragma once

#include <string>

namespace lab5 {

struct Record {
    int id = 0;
    std::string name;
    double score = 0.0;

    bool operator==(const Record&) const = default;
};

} // namespace lab5
