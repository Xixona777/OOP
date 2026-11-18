#pragma once

#include <string>
#include <string_view>

class BattleVisitor;
class Elf;
class Dragon;
class Druid;

enum class NpcType {
    Elf,
    Dragon,
    Druid,
};

std::string_view toString(NpcType type) noexcept;

class Npc {
public:
    Npc(NpcType type, std::string name, double x, double y);
    virtual ~Npc() = default;

    Npc(const Npc&) = delete;
    Npc& operator=(const Npc&) = delete;
    Npc(Npc&&) = delete;
    Npc& operator=(Npc&&) = delete;

    NpcType type() const noexcept;
    const std::string& name() const noexcept;
    double x() const noexcept;
    double y() const noexcept;
    bool isAlive() const noexcept;
    void kill() noexcept;
    double distanceTo(const Npc& other) const noexcept;

    virtual void accept(BattleVisitor& visitor) = 0;
    virtual void accept(BattleVisitor& visitor, Elf& attacker) = 0;
    virtual void accept(BattleVisitor& visitor, Dragon& attacker) = 0;
    virtual void accept(BattleVisitor& visitor, Druid& attacker) = 0;

private:
    NpcType type_;
    std::string name_;
    double x_;
    double y_;
    bool alive_ = true;
};

class Elf final : public Npc {
public:
    Elf(std::string name, double x, double y);

    void accept(BattleVisitor& visitor) override;
    void accept(BattleVisitor& visitor, Elf& attacker) override;
    void accept(BattleVisitor& visitor, Dragon& attacker) override;
    void accept(BattleVisitor& visitor, Druid& attacker) override;
};

class Dragon final : public Npc {
public:
    Dragon(std::string name, double x, double y);

    void accept(BattleVisitor& visitor) override;
    void accept(BattleVisitor& visitor, Elf& attacker) override;
    void accept(BattleVisitor& visitor, Dragon& attacker) override;
    void accept(BattleVisitor& visitor, Druid& attacker) override;
};

class Druid final : public Npc {
public:
    Druid(std::string name, double x, double y);

    void accept(BattleVisitor& visitor) override;
    void accept(BattleVisitor& visitor, Elf& attacker) override;
    void accept(BattleVisitor& visitor, Dragon& attacker) override;
    void accept(BattleVisitor& visitor, Druid& attacker) override;
};
