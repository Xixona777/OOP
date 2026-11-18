#include "simulation.h"

#include <chrono>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void printUsage(const char* executable) {
    std::cout << "Usage: " << executable
              << " [--duration-ms N] [--step-ms N] [--count N] [--seed N]"
                 " [--log PATH] [--no-map]\n";
}

int parseNonNegative(const std::string& value, const std::string& option) {
    std::size_t parsed = 0;
    const long long result = std::stoll(value, &parsed);
    if (parsed != value.size() || result < 0) {
        throw std::invalid_argument("Invalid value for " + option + ": " + value);
    }
    return static_cast<int>(result);
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        lab7::SimulationConfig config;
        for (int index = 1; index < argc; ++index) {
            const std::string argument = argv[index];
            if (argument == "--help") {
                printUsage(argv[0]);
                return 0;
            }
            if (argument == "--no-map") {
                config.printMaps = false;
                continue;
            }
            if (index + 1 >= argc) {
                throw std::invalid_argument("Missing value for " + argument);
            }
            const std::string value = argv[++index];
            if (argument == "--duration-ms") {
                config.duration =
                    std::chrono::milliseconds(parseNonNegative(value, argument));
            } else if (argument == "--step-ms") {
                config.movementInterval =
                    std::chrono::milliseconds(parseNonNegative(value, argument));
            } else if (argument == "--count") {
                config.initialNpcCount =
                    static_cast<std::size_t>(parseNonNegative(value, argument));
            } else if (argument == "--seed") {
                config.seed =
                    static_cast<std::uint32_t>(parseNonNegative(value, argument));
            } else if (argument == "--log") {
                config.logFile = value;
            } else {
                throw std::invalid_argument("Unknown option: " + argument);
            }
        }

        std::cout << "Lab 7, variant 10: " << config.initialNpcCount
                  << " NPC, map 100 x 100, duration "
                  << config.duration.count() << " ms, seed " << config.seed << '\n';
        lab7::Simulation simulation(config);
        simulation.run();
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << '\n';
        printUsage(argv[0]);
        return 1;
    }
}
