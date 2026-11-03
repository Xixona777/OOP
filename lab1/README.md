# Лабораторная работа №1

**Задание:** реализовать функцию, которая возвращает количество английских гласных `a`, `e`, `i`, `o`, `u` в строке. Строка содержит только строчные английские буквы и пробелы.

Теория и объяснение кода находятся в [THEORY.pdf](THEORY.pdf).

## Порядок выполнения работы

1. Создал в каталоге проекта папки `include`, `src` и `tests`. Каталог `build` вручную не создавал: его создаёт CMake.
2. В `include/vowels.hpp` объявил функцию `countVowels`.
3. В `src/vowels.cpp` реализовал подсчёт гласных с помощью цикла по символам строки и оператора `switch`.
4. В `main.cpp` организовал чтение всей строки через `std::getline`, вызов функции и вывод результата.
5. В `tests/vowels_test.cpp` написал пять тестов Google Test:
   - пустая строка;
   - строка без гласных;
   - строка только из гласных;
   - строка с пробелами;
   - обычный текст.
6. В `CMakeLists.txt` настроил стандарт C++17, проверку GCC 14, сборку программы и библиотеки, загрузку Google Test и запуск тестов через CTest.
7. Установил GCC 14 и CMake, затем открыл CMD и перешёл в каталог проекта:

   ```bat
   cd /d C:\Projects\OOP\lab1
   ```

8. Выполнил настройку проекта. При первой настройке CMake автоматически загрузил Google Test:

   ```bat
   cmake -S . -B build -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
   ```

9. Собрал проект:

   ```bat
   cmake --build build
   ```

10. Запустил тесты:

    ```bat
    ctest --test-dir build --output-on-failure
    ```

    Результат: `100% tests passed, 0 tests failed out of 5`.

11. Проверил работу программы:

    ```bat
    build\vowels_app.exe
    ```

    После запуска ввёл строку и нажал Enter. Например, для строки `hello world` программа выводит `3`.

## Команды для повторного запуска

Из каталога `C:\Projects\OOP\lab1`:

```bat
cmake --build build
ctest --test-dir build --output-on-failure
build\vowels_app.exe
```
