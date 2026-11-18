#pragma once

#include "Npc.h"

class BattleVisitor {
public:
    bool canKill(Npc& attacker, Npc& defender);

    void visit(Elf& attacker);
    void visit(Dragon& attacker);
    void visit(Druid& attacker);

    void visit(Npc& attacker, Elf& defender) noexcept;
    void visit(Npc& attacker, Dragon& defender) noexcept;
    void visit(Npc& attacker, Druid& defender) noexcept;
    void visit(Elf& attacker, Elf& defender) noexcept;
    void visit(Elf& attacker, Dragon& defender) noexcept;
    void visit(Elf& attacker, Druid& defender) noexcept;
    void visit(Dragon& attacker, Elf& defender) noexcept;
    void visit(Dragon& attacker, Dragon& defender) noexcept;
    void visit(Dragon& attacker, Druid& defender) noexcept;
    void visit(Druid& attacker, Elf& defender) noexcept;
    void visit(Druid& attacker, Dragon& defender) noexcept;
    void visit(Druid& attacker, Druid& defender) noexcept;

private:
    Npc* defender_ = nullptr;
    bool kills_ = false;
};
