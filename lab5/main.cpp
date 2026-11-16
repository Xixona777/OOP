#include "fixed_block_memory_resource.hpp"
#include "record.hpp"
#include "singly_linked_list.hpp"

#include <iomanip>
#include <iostream>
#include <iterator>

namespace {

void print_statistics(const lab5::FixedBlockMemoryResource& resource) {
    std::cout << "Статистика: занято участков = " << resource.occupied_block_count()
              << ", свободно участков = " << resource.free_block_count()
              << ", обращений allocate к upstream = "
              << resource.upstream_allocation_count() << '\n';
}

void print_records(const lab5::SinglyLinkedList<lab5::Record>& records) {
    for (const auto& record : records) {
        std::cout << "  { id = " << record.id << ", name = \"" << record.name
                  << "\", score = " << std::fixed << std::setprecision(1)
                  << record.score << " }\n";
    }
}

} // namespace

int main() {
    constexpr std::size_t buffer_capacity = 16 * 1024;
    lab5::FixedBlockMemoryResource resource(buffer_capacity);

    std::cout << "=== Однонаправленный список и fixed-block memory_resource ===\n";
    std::cout << "Единый буфер: " << buffer_capacity << " байт\n";
    print_statistics(resource);

    {
        lab5::SinglyLinkedList<int> numbers(&resource);
        numbers.push_back(2);
        numbers.push_back(3);
        numbers.push_front(1);
        numbers.push_back(4);

        std::cout << "\nЦелые числа после добавления: ";
        for (int value : numbers) {
            std::cout << value << ' ';
        }
        std::cout << "\nstd::distance(begin, end) = "
                  << std::distance(numbers.begin(), numbers.end()) << '\n';

        auto first = numbers.begin();
        auto independent_copy = first;
        ++first;
        std::cout << "Независимые копии итератора: " << *first << " и "
                  << *independent_copy << '\n';

        const int* released_value_address = &numbers.front();
        numbers.pop_front();
        numbers.push_front(10);
        std::cout << "После pop_front и push_front: ";
        for (auto it = numbers.cbegin(); it != numbers.cend(); ++it) {
            std::cout << *it << ' ';
        }
        std::cout << "\nАдрес узла повторно использован после pop_front: "
                  << std::boolalpha
                  << (released_value_address == &numbers.front()) << '\n';
        print_statistics(resource);

        numbers.clear();
        numbers.push_back(7);
        numbers.push_back(8);
        std::cout << "После clear и повторного заполнения: ";
        for (int value : numbers) {
            std::cout << value << ' ';
        }
        std::cout << '\n';
    }

    std::cout << "\nСписок<int> уничтожен до ресурса.\n";
    print_statistics(resource);

    {
        lab5::SinglyLinkedList<lab5::Record> records(&resource);
        records.push_back(lab5::Record{1, "Анна", 91.5});
        records.push_back(lab5::Record{2, "Борис", 84.0});
        records.push_front(lab5::Record{3, "Вера", 96.5});

        std::cout << "\nСложные объекты Record:\n";
        print_records(records);
        print_statistics(resource);

        records.pop_front();
        std::cout << "После удаления первой записи:\n";
        print_records(records);

        records.clear();
        records.push_back(lab5::Record{4, "Григорий", 88.5});
        std::cout << "После clear и добавления новой записи:\n";
        print_records(records);
    }

    std::cout << "\nСписок<Record> уничтожен до ресурса.\n";
    print_statistics(resource);
    std::cout << "Демонстрация завершена, теперь будет уничтожен memory_resource.\n";
}
