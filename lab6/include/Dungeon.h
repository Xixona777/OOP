#pragma once

#include "Npc.h"
#include "NpcFactory.h"
#include "Observers.h"

#include <cstddef>
#include <filesystem>
#include <iosfwd>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class Dungeon {
public:
    using NpcList = std::vector<std::unique_ptr<Npc>>;

    Npc& add(NpcType type, const std::string& name, double x, double y);
    Npc& add(std::string_view type, const std::string& name, double x, double y);

    void save(const std::filesystem::path& filename) const;
    void load(const std::filesystem::path& filename);
    void print(std::ostream& output) const;
    std::size_t fight(double range);

    void addObserver(const std::shared_ptr<IObserver>& observer);
    void removeObserver(const std::shared_ptr<IObserver>& observer);

    std::size_t size() const noexcept;
    bool empty() const noexcept;
    const NpcList& npcs() const noexcept;

private:
    void notifyKill(const Npc& killer, const Npc& victim);

    NpcFactory factory_;
    NpcList npcs_;
    std::vector<std::weak_ptr<IObserver>> observers_;
};
