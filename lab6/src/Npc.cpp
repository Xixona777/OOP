#include "Npc.h"

#include "BattleVisitor.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <utility>

std::string_view toString(NpcType type) noexcept {
    switch (type) {
    case NpcType::Elf:
        return "Elf";
    case NpcType::Dragon:
        return "Dragon";
    case NpcType::Druid:
        return "Druid";
    }
    return "Unknown";
}

Npc::Npc(NpcType type, std::string name, double x, double y)
    : type_(type), name_(std::move(name)), x_(x), y_(y) {
    if (name_.empty()) {
        throw std::invalid_argument("NPC name must not be empty");
    }
    if (std::any_of(name_.begin(), name_.end(), [](unsigned char ch) {
            return std::isspace(ch) != 0;
        })) {
        throw std::invalid_argument("NPC name must not contain whitespace");
    }
    if (!std::isfinite(x_) || !std::isfinite(y_) ||
        x_ < 0.0 || x_ > 500.0 || y_ < 0.0 || y_ > 500.0) {
        throw std::invalid_argument("NPC coordinates must be finite and within [0, 500]");
    }
}

NpcType Npc::type() const noexcept {
    return type_;
}

const std::string& Npc::name() const noexcept {
    return name_;
}

double Npc::x() const noexcept {
    return x_;
}

double Npc::y() const noexcept {
    return y_;
}

bool Npc::isAlive() const noexcept {
    return alive_;
}

void Npc::kill() noexcept {
    alive_ = false;
}

double Npc::distanceTo(const Npc& other) const noexcept {
    return std::hypot(x_ - other.x_, y_ - other.y_);
}

Elf::Elf(std::string name, double x, double y)
    : Npc(NpcType::Elf, std::move(name), x, y) {}

void Elf::accept(BattleVisitor& visitor) {
    visitor.visit(*this);
}

void Elf::accept(BattleVisitor& visitor, Elf& attacker) {
    visitor.visit(attacker, *this);
}

void Elf::accept(BattleVisitor& visitor, Dragon& attacker) {
    visitor.visit(attacker, *this);
}

void Elf::accept(BattleVisitor& visitor, Druid& attacker) {
    visitor.visit(attacker, *this);
}

Dragon::Dragon(std::string name, double x, double y)
    : Npc(NpcType::Dragon, std::move(name), x, y) {}

void Dragon::accept(BattleVisitor& visitor) {
    visitor.visit(*this);
}

void Dragon::accept(BattleVisitor& visitor, Elf& attacker) {
    visitor.visit(attacker, *this);
}

void Dragon::accept(BattleVisitor& visitor, Dragon& attacker) {
    visitor.visit(attacker, *this);
}

void Dragon::accept(BattleVisitor& visitor, Druid& attacker) {
    visitor.visit(attacker, *this);
}

Druid::Druid(std::string name, double x, double y)
    : Npc(NpcType::Druid, std::move(name), x, y) {}

void Druid::accept(BattleVisitor& visitor) {
    visitor.visit(*this);
}

void Druid::accept(BattleVisitor& visitor, Elf& attacker) {
    visitor.visit(attacker, *this);
}

void Druid::accept(BattleVisitor& visitor, Dragon& attacker) {
    visitor.visit(attacker, *this);
}

void Druid::accept(BattleVisitor& visitor, Druid& attacker) {
    visitor.visit(attacker, *this);
}
