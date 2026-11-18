#pragma once

#include "Npc.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onKill(const Npc& killer, const Npc& victim) = 0;
};

class ConsoleObserver final : public IObserver {
public:
    explicit ConsoleObserver(std::ostream& output = std::cout);

    void onKill(const Npc& killer, const Npc& victim) override;

private:
    std::ostream& output_;
};

class FileObserver final : public IObserver {
public:
    explicit FileObserver(const std::filesystem::path& filename = "log.txt");

    void onKill(const Npc& killer, const Npc& victim) override;

private:
    std::ofstream output_;
};
