#include "figure_array.hpp"
#include "hexagon.hpp"
#include "octagon.hpp"
#include "pentagon.hpp"

#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

using namespace lab3;

void ignoreLine(std::istream& input) {
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void addFigure(FigureArray& figures, int sides) {
    std::unique_ptr<Figure> figure;
    if (sides == 5) {
        figure = std::make_unique<Pentagon>();
    } else if (sides == 6) {
        figure = std::make_unique<Hexagon>();
    } else if (sides == 8) {
        figure = std::make_unique<Octagon>();
    } else {
        std::cout << "Ошибка: поддерживаются только фигуры с 5, 6 или 8 вершинами.\n";
        ignoreLine(std::cin);
        return;
    }

    try {
        std::cin >> *figure;
        if (!std::cin) {
            std::cout << "Ошибка: не удалось прочитать координаты.\n";
            std::cin.clear();
            ignoreLine(std::cin);
            return;
        }
        figures.add(figure.release());
        std::cout << "Фигура добавлена. Индекс: " << figures.size() - 1 << '\n';
    } catch (const std::invalid_argument& error) {
        std::cout << "Ошибка: " << error.what() << ". Фигура не добавлена.\n";
    }
}

void listFigures(const FigureArray& figures) {
    if (figures.empty()) {
        std::cout << "Массив пуст.\n";
        return;
    }
    for (std::size_t i = 0; i < figures.size(); ++i) {
        const Figure* figure = figures.at(i);
        const Point center = figure->center();
        std::cout << '[' << i << "] " << figure->typeName() << '\n'
                  << "Вершины: " << *figure << '\n'
                  << "Центр: (" << center.x << ", " << center.y << ")\n"
                  << "Площадь: " << static_cast<double>(*figure) << '\n';
    }
}

}  // namespace

int main() {
    std::cout << std::setprecision(10);
    FigureArray figures;
    std::string command;

    while (std::cin >> command) {
        if (command == "exit") {
            break;
        }
        if (command == "add") {
            int sides = 0;
            if (!(std::cin >> sides)) {
                std::cout << "Ошибка: после add требуется число вершин.\n";
                std::cin.clear();
                ignoreLine(std::cin);
                continue;
            }
            addFigure(figures, sides);
        } else if (command == "list") {
            listFigures(figures);
        } else if (command == "total") {
            std::cout << "Суммарная площадь: " << figures.totalArea() << '\n';
        } else if (command == "remove") {
            std::size_t index = 0;
            if (!(std::cin >> index)) {
                std::cout << "Ошибка: после remove требуется индекс.\n";
                std::cin.clear();
                ignoreLine(std::cin);
                continue;
            }
            try {
                figures.remove(index);
                std::cout << "Фигура удалена.\n";
            } catch (const std::out_of_range& error) {
                std::cout << "Ошибка: " << error.what() << '\n';
            }
        } else {
            std::cout << "Ошибка: неизвестная команда '" << command << "'.\n";
            ignoreLine(std::cin);
        }
    }

    return 0;
}
