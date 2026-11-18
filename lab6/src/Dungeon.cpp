#include "Dungeon.h"

#include "BattleVisitor.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

Npc& Dungeon::add(NpcType type, const std::string& name, double x, double y) {
    const auto duplicate = std::find_if(npcs_.begin(), npcs_.end(),
                                        [&name](const auto& npc) {
                                            return npc->name() == name;
                                        });
    if (duplicate != npcs_.end()) {
        throw std::invalid_argument("NPC name already exists: " + name);
    }

    auto npc = factory_.create(type, name, x, y);
    Npc& result = *npc;
    npcs_.push_back(std::move(npc));
    return result;
}

Npc& Dungeon::add(std::string_view type, const std::string& name,
                  double x, double y) {
    const auto duplicate = std::find_if(npcs_.begin(), npcs_.end(),
                                        [&name](const auto& npc) {
                                            return npc->name() == name;
                                        });
    if (duplicate != npcs_.end()) {
        throw std::invalid_argument("NPC name already exists: " + name);
    }

    auto npc = factory_.create(type, name, x, y);
    Npc& result = *npc;
    npcs_.push_back(std::move(npc));
    return result;
}

void Dungeon::save(const std::filesystem::path& filename) const {
    std::ofstream output(filename, std::ios::trunc);
    if (!output.is_open()) {
        throw std::runtime_error("Cannot open file for saving: " + filename.string());
    }

    output << std::setprecision(17);
    for (const auto& npc : npcs_) {
        output << toString(npc->type()) << ' ' << npc->name() << ' '
               << npc->x() << ' ' << npc->y() << '\n';
    }
    if (!output) {
        throw std::runtime_error("Cannot write NPC data to: " + filename.string());
    }
}

void Dungeon::load(const std::filesystem::path& filename) {
    std::ifstream input(filename);
    if (!input.is_open()) {
        throw std::runtime_error("Cannot open file for loading: " + filename.string());
    }

    Dungeon loaded;
    std::string line;
    std::size_t lineNumber = 0;
    try {
        while (std::getline(input, line)) {
            ++lineNumber;
            std::istringstream lineStream(line);
            std::string type;
            if (!(lineStream >> type)) {
                continue;
            }

            std::string name;
            double x = 0.0;
            double y = 0.0;
            std::string extra;
            if (!(lineStream >> name >> x >> y) || (lineStream >> extra)) {
                throw std::runtime_error("Expected exactly: type name x y");
            }
            loaded.add(type, name, x, y);
        }
        if (input.bad()) {
            throw std::runtime_error("Error while reading file");
        }
    } catch (const std::exception& error) {
        throw std::runtime_error("Cannot load " + filename.string() + " at line " +
                                 std::to_string(lineNumber) + ": " + error.what());
    }

    npcs_ = std::move(loaded.npcs_);
}

void Dungeon::print(std::ostream& output) const {
    if (npcs_.empty()) {
        output << "Dungeon is empty\n";
        return;
    }
    for (const auto& npc : npcs_) {
        output << toString(npc->type()) << ' ' << npc->name() << ' '
               << npc->x() << ' ' << npc->y() << '\n';
    }
}

std::size_t Dungeon::fight(double range) {
    if (!std::isfinite(range) || range < 0.0) {
        throw std::invalid_argument("Battle range must be a finite non-negative number");
    }

    BattleVisitor visitor;
    std::size_t killCount = 0;
    for (std::size_t i = 0; i < npcs_.size(); ++i) {
        for (std::size_t j = i + 1; j < npcs_.size(); ++j) {
            Npc& first = *npcs_[i];
            Npc& second = *npcs_[j];
            if (!first.isAlive() || !second.isAlive() ||
                first.distanceTo(second) > range) {
                continue;
            }

            const bool firstKillsSecond = visitor.canKill(first, second);
            const bool secondKillsFirst = visitor.canKill(second, first);

            if (firstKillsSecond) {
                second.kill();
                notifyKill(first, second);
                ++killCount;
            }
            if (secondKillsFirst) {
                first.kill();
                notifyKill(second, first);
                ++killCount;
            }
        }
    }

    std::erase_if(npcs_, [](const auto& npc) { return !npc->isAlive(); });
    return killCount;
}

void Dungeon::addObserver(const std::shared_ptr<IObserver>& observer) {
    if (!observer) {
        throw std::invalid_argument("Observer must not be null");
    }
    observers_.push_back(observer);
}

void Dungeon::removeObserver(const std::shared_ptr<IObserver>& observer) {
    std::erase_if(observers_, [&observer](const auto& weakObserver) {
        const auto current = weakObserver.lock();
        return !current || current == observer;
    });
}

void Dungeon::notifyKill(const Npc& killer, const Npc& victim) {
    std::vector<std::shared_ptr<IObserver>> activeObservers;
    for (const auto& weakObserver : observers_) {
        if (auto observer = weakObserver.lock()) {
            activeObservers.push_back(std::move(observer));
        }
    }
    std::erase_if(observers_, [](const auto& observer) {
        return observer.expired();
    });

    for (const auto& observer : activeObservers) {
        observer->onKill(killer, victim);
    }
}

std::size_t Dungeon::size() const noexcept {
    return npcs_.size();
}

bool Dungeon::empty() const noexcept {
    return npcs_.empty();
}

const Dungeon::NpcList& Dungeon::npcs() const noexcept {
    return npcs_;
}
