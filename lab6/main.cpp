#include "Dungeon.h"
#include "Observers.h"

#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

int main() {
    Dungeon dungeon;
    auto consoleObserver = std::make_shared<ConsoleObserver>();
    dungeon.addObserver(consoleObserver);

    std::shared_ptr<FileObserver> fileObserver;
    try {
        fileObserver = std::make_shared<FileObserver>("log.txt");
        dungeon.addObserver(fileObserver);
    } catch (const std::exception& error) {
        std::cerr << "File observer disabled: " << error.what() << '\n';
    }

    std::cout << "Dungeon editor. Commands: add, print, save, load, fight, exit\n";
    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        std::istringstream input(line);
        std::string command;
        if (!(input >> command)) {
            continue;
        }

        try {
            if (command == "exit") {
                break;
            }
            if (command == "add") {
                std::string type;
                std::string name;
                double x = 0.0;
                double y = 0.0;
                std::string extra;
                if (!(input >> type >> name >> x >> y) || (input >> extra)) {
                    throw std::invalid_argument("Usage: add Elf|Dragon|Druid name x y");
                }
                dungeon.add(type, name, x, y);
                std::cout << "Added " << type << ' ' << name << '\n';
            } else if (command == "print") {
                dungeon.print(std::cout);
            } else if (command == "save") {
                std::string path;
                std::string extra;
                if (!(input >> path) || (input >> extra)) {
                    throw std::invalid_argument("Usage: save path");
                }
                dungeon.save(path);
                std::cout << "Saved " << dungeon.size() << " NPCs to " << path << '\n';
            } else if (command == "load") {
                std::string path;
                std::string extra;
                if (!(input >> path) || (input >> extra)) {
                    throw std::invalid_argument("Usage: load path");
                }
                dungeon.load(path);
                std::cout << "Loaded " << dungeon.size() << " NPCs from " << path << '\n';
            } else if (command == "fight") {
                double range = 0.0;
                std::string extra;
                if (!(input >> range) || (input >> extra)) {
                    throw std::invalid_argument("Usage: fight non-negative-distance");
                }
                const std::size_t kills = dungeon.fight(range);
                std::cout << "Battle finished. Killed: " << kills
                          << ". Remaining: " << dungeon.size() << '\n';
            } else {
                std::cerr << "Unknown command: " << command << '\n';
            }
        } catch (const std::exception& error) {
            std::cerr << "Error: " << error.what() << '\n';
        }
    }
    return 0;
}
