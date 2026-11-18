#include "Observers.h"

#include <stdexcept>
#include <string>

ConsoleObserver::ConsoleObserver(std::ostream& output) : output_(output) {}

void ConsoleObserver::onKill(const Npc& killer, const Npc& victim) {
    output_ << toString(killer.type()) << ' ' << killer.name() << " killed "
            << toString(victim.type()) << ' ' << victim.name() << '\n';
}

FileObserver::FileObserver(const std::filesystem::path& filename) {
    output_.open(filename, std::ios::app);
    if (!output_.is_open()) {
        throw std::runtime_error("Cannot open log file: " + filename.string());
    }
}

void FileObserver::onKill(const Npc& killer, const Npc& victim) {
    output_ << toString(killer.type()) << ' ' << killer.name() << " killed "
            << toString(victim.type()) << ' ' << victim.name() << '\n';
    output_.flush();
    if (!output_) {
        throw std::runtime_error("Cannot write kill event to log file");
    }
}
