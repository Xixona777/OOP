#include "npc.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace lab7 {

std::string_view toString(NpcType type) noexcept {
    switch (type) {
    case NpcType::Robber:
        return "Robber";
    case NpcType::Orc:
        return "Orc";
    case NpcType::Werewolf:
        return "Werewolf";
    }
    return "Unknown";
}

char symbol(NpcType type) noexcept {
    switch (type) {
    case NpcType::Robber:
        return 'R';
    case NpcType::Orc:
        return 'O';
    case NpcType::Werewolf:
        return 'W';
    }
    return '?';
}

NPC::NPC(NpcType type, std::string name, Position position, int moveDistance,
         int killDistance)
    : type_(type), name_(std::move(name)), position_(position),
      moveDistance_(moveDistance), killDistance_(killDistance) {
    if (name_.empty()) {
        throw std::invalid_argument("NPC name must not be empty");
    }
    setPosition(position_);
}

const std::string& NPC::name() const noexcept {
    return name_;
}

NpcType NPC::type() const noexcept {
    return type_;
}

Position NPC::position() const noexcept {
    return position_;
}

bool NPC::isAlive() const noexcept {
    return alive_;
}

int NPC::moveDistance() const noexcept {
    return moveDistance_;
}

int NPC::killDistance() const noexcept {
    return killDistance_;
}

void NPC::setPosition(Position position) {
    if (position.x < 0 || position.x > 100 || position.y < 0 || position.y > 100) {
        throw std::out_of_range("NPC position is outside the 100 x 100 map");
    }
    position_ = position;
}

void NPC::kill() noexcept {
    alive_ = false;
}

Robber::Robber(std::string name, Position position)
    : NPC(NpcType::Robber, std::move(name), position, 10, 10) {}

bool Robber::canKill(const NPC& other) const noexcept {
    return other.type() == NpcType::Werewolf;
}

Orc::Orc(std::string name, Position position)
    : NPC(NpcType::Orc, std::move(name), position, 20, 10) {}

bool Orc::canKill(const NPC& other) const noexcept {
    return other.type() == NpcType::Robber;
}

Werewolf::Werewolf(std::string name, Position position)
    : NPC(NpcType::Werewolf, std::move(name), position, 40, 5) {}

bool Werewolf::canKill(const NPC& other) const noexcept {
    return other.type() == NpcType::Robber;
}

double distance(const NPC& first, const NPC& second) noexcept {
    const double dx = static_cast<double>(first.position().x - second.position().x);
    const double dy = static_cast<double>(first.position().y - second.position().y);
    return std::sqrt(dx * dx + dy * dy);
}

} // namespace lab7
