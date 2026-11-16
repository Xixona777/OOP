#include "Demo.hpp"

#include "Array.hpp"
#include "Figure.hpp"
#include "Hexagon.hpp"
#include "Octagon.hpp"
#include "Triangle.hpp"

#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>

namespace {
using FigurePtr = std::shared_ptr<Figure<double>>;

void printFigures(const Array<FigurePtr>& figures) {
    if (figures.size() == 0) {
        std::cout << "Список фигур пуст.\n";
        return;
    }

    std::cout << std::setprecision(10);
    for (std::size_t i = 0; i < figures.size(); ++i) {
        const auto& figure = *figures[i];
        const auto center = figure.center();
        std::cout << '[' << i << "] " << figure.type()
                  << "\n  Вершины: " << figure
                  << "\n  Центр: (" << center.x << ", " << center.y << ')'
                  << "\n  Площадь: " << static_cast<double>(figure) << '\n';
    }
}

double totalArea(const Array<FigurePtr>& figures) {
    double result = 0.0;
    for (std::size_t i = 0; i < figures.size(); ++i) {
        result += static_cast<double>(*figures[i]);
    }
    return result;
}

void clearFailedInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void addFigure(Array<FigurePtr>& figures, int sides) {
    FigurePtr figure;
    if (sides == 6) {
        figure = std::make_shared<Hexagon<double>>();
    } else if (sides == 8) {
        figure = std::make_shared<Octagon<double>>();
    } else if (sides == 3) {
        figure = std::make_shared<Triangle<double>>();
    } else {
        std::cout << "Допустимы только команды add 6, add 8 и add 3.\n";
        return;
    }

    std::cout << "Введите " << sides << " пар координат x y в порядке обхода: ";
    std::cin >> *figure;
    if (!std::cin) {
        std::cout << "Фигура не добавлена: ожидались координаты правильного "
                     "невырожденного выпуклого многоугольника.\n";
        clearFailedInput();
        return;
    }

    figures.push_back(std::move(figure));
    std::cout << "Фигура добавлена.\n";
}
}

void demonstrateTemplateArrays() {
    std::cout << "=== Демонстрация Array<std::shared_ptr<Figure<int>>> ===\n";
    Array<std::shared_ptr<Figure<int>>> polymorphic;
    polymorphic.push_back(std::make_shared<Hexagon<int>>());
    std::shared_ptr<Figure<int>> independentCopy = polymorphic[0]->clone();
    polymorphic.push_back(std::move(independentCopy));
    for (std::size_t i = 0; i < polymorphic.size(); ++i) {
        std::cout << '[' << i << "] " << polymorphic[i]->type() << ": "
                  << *polymorphic[i] << ", площадь = "
                  << static_cast<double>(*polymorphic[i]) << '\n';
    }

    std::cout << "=== Демонстрация Array<Hexagon<int>> ===\n";
    Array<Hexagon<int>> concrete;
    Hexagon<int> original;
    concrete.push_back(original);
    Hexagon<int> moved;
    concrete.push_back(std::move(moved));
    for (std::size_t i = 0; i < concrete.size(); ++i) {
        const auto center = concrete[i].center();
        std::cout << '[' << i << "] " << concrete[i]
                  << ", центр = (" << center.x << ", " << center.y << ')' << '\n';
    }
    std::cout << "Целочисленные объекты созданы конструктором по умолчанию; "
                 "корректные геометрические фигуры ниже используют double.\n\n";
}

void runInteractiveDemo() {
    Array<FigurePtr> figures;
    std::cout << "Команды: add 6 | add 8 | add 3 | list | total | remove N | exit\n";

    std::string command;
    while (std::cout << "> " && std::cin >> command) {
        if (command == "exit") {
            break;
        }
        if (command == "list") {
            printFigures(figures);
        } else if (command == "total") {
            std::cout << "Общая площадь: " << totalArea(figures) << '\n';
        } else if (command == "add") {
            int sides = 0;
            if (std::cin >> sides) {
                addFigure(figures, sides);
            } else {
                std::cout << "После add укажите 6, 8 или 3.\n";
                clearFailedInput();
            }
        } else if (command == "remove") {
            std::size_t index = 0;
            if (!(std::cin >> index)) {
                std::cout << "После remove укажите неотрицательный индекс.\n";
                clearFailedInput();
                continue;
            }
            try {
                figures.remove(index);
                std::cout << "Фигура удалена.\n";
            } catch (const std::out_of_range& error) {
                std::cout << error.what() << '\n';
            }
        } else {
            std::cout << "Неизвестная команда.\n";
        }
    }
}
