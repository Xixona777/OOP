#include "BattleVisitor.h"

bool BattleVisitor::canKill(Npc& attacker, Npc& defender) {
    defender_ = &defender;
    kills_ = false;
    attacker.accept(*this);
    defender_ = nullptr;
    return kills_;
}

void BattleVisitor::visit(Elf& attacker) {
    defender_->accept(*this, attacker);
}

void BattleVisitor::visit(Dragon& attacker) {
    defender_->accept(*this, attacker);
}

void BattleVisitor::visit(Druid& attacker) {
    defender_->accept(*this, attacker);
}

void BattleVisitor::visit(Npc&, Elf&) noexcept {
    kills_ = false;
}

void BattleVisitor::visit(Npc&, Dragon&) noexcept {
    kills_ = false;
}

void BattleVisitor::visit(Npc&, Druid&) noexcept {
    kills_ = false;
}

void BattleVisitor::visit(Elf&, Elf&) noexcept {
    kills_ = false;
}

void BattleVisitor::visit(Elf&, Dragon&) noexcept {
    kills_ = false;
}

void BattleVisitor::visit(Elf&, Druid&) noexcept {
    kills_ = true;
}

void BattleVisitor::visit(Dragon&, Elf&) noexcept {
    kills_ = true;
}

void BattleVisitor::visit(Dragon&, Dragon&) noexcept {
    kills_ = false;
}

void BattleVisitor::visit(Dragon&, Druid&) noexcept {
    kills_ = false;
}

void BattleVisitor::visit(Druid&, Elf&) noexcept {
    kills_ = false;
}

void BattleVisitor::visit(Druid&, Dragon&) noexcept {
    kills_ = true;
}

void BattleVisitor::visit(Druid&, Druid&) noexcept {
    kills_ = false;
}
