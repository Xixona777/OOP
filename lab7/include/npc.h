#pragma once

#include <string>
#include <string_view>

namespace lab7 {

enum class NpcType {
    Robber,
    Orc,
    Werewolf,
};

struct Position {
    int x = 0;
    int y = 0;

    friend bool operator==(const Position&, const Position&) = default;
};

std::string_view toString(NpcType type) noexcept;
char symbol(NpcType type) noexcept;

class NPC {
public:
    NPC(NpcType type, std::string name, Position position, int moveDistance,
        int killDistance);
    virtual ~NPC() = default;

    NPC(const NPC&) = delete;
    NPC& operator=(const NPC&) = delete;

    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] NpcType type() const noexcept;
    [[nodiscard]] Position position() const noexcept;
    [[nodiscard]] bool isAlive() const noexcept;
    [[nodiscard]] int moveDistance() const noexcept;
    [[nodiscard]] int killDistance() const noexcept;

    void setPosition(Position position);
    void kill() noexcept;

    [[nodiscard]] virtual bool canKill(const NPC& other) const noexcept = 0;

private:
    NpcType type_;
    std::string name_;
    Position position_;
    bool alive_ = true;
    int moveDistance_;
    int killDistance_;
};

class Robber final : public NPC {
public:
    Robber(std::string name, Position position);

    [[nodiscard]] bool canKill(const NPC& other) const noexcept override;
};

class Orc final : public NPC {
public:
    Orc(std::string name, Position position);

    [[nodiscard]] bool canKill(const NPC& other) const noexcept override;
};

class Werewolf final : public NPC {
public:
    Werewolf(std::string name, Position position);

    [[nodiscard]] bool canKill(const NPC& other) const noexcept override;
};

[[nodiscard]] double distance(const NPC& first, const NPC& second) noexcept;

} // namespace lab7
