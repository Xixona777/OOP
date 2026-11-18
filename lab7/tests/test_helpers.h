#pragma once

#include "dice.h"
#include "observer.h"

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

class SequenceDice final : public lab7::IDice {
public:
    explicit SequenceDice(std::vector<int> values) : values_(std::move(values)) {}

    int roll() override {
        if (index_ >= values_.size()) {
            throw std::out_of_range("SequenceDice has no values left");
        }
        return values_[index_++];
    }

    [[nodiscard]] std::size_t rolls() const noexcept {
        return index_;
    }

private:
    std::vector<int> values_;
    std::size_t index_ = 0;
};

class RecordingObserver final : public lab7::Observer {
public:
    void onKill(const lab7::KillEvent& event) override {
        events.push_back(event);
    }

    std::vector<lab7::KillEvent> events;
};
