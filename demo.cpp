#include <iostream>
#include <utility>

#include "OrderedList.h"

int main() {
    using IP2::OrderedList;

    OrderedList numbers;
    numbers += 7;
    numbers += 3;
    numbers += 9;
    numbers += 7;
    numbers.insert(1);

    std::cout << "Initial list: " << numbers.toString() << '\n';
    std::cout << "Element at index 2: " << numbers.at(2) << '\n';
    std::cout << "Contains 9: " << (numbers.contains(9) ? "yes" : "no") << '\n';
    std::cout << "Count of 7: " << numbers.count(7) << '\n';
    std::cout << "find(3): " << numbers.find(3) << '\n';
    std::cout << "Position of 7: " << numbers[7] << '\n';
    std::cout << "Size before update: " << numbers.size() << '\n';
    std::cout << "Comparator is ascending: "
              << (numbers.getComparator() == OrderedList::ascending ? "yes" : "no") << '\n';

    numbers.updateAt(0, 5);
    std::cout << "After direct updateAt(0, 5): " << numbers.toString() << '\n';

    numbers *= OrderedList::EditCommand{1, 8};
    std::cout << "After editing index 1 to 8: " << numbers.toString() << '\n';

    numbers -= 7;
    std::cout << "After deleting one 7: " << numbers.toString() << '\n';

    OrderedList copied(numbers);
    std::cout << "Copied list: " << copied.toString() << '\n';
    std::cout << "numbers == copied: " << (numbers == copied ? "true" : "false") << '\n';

    OrderedList assigned;
    assigned = copied;
    std::cout << "Copy-assigned list: " << assigned.toString() << '\n';

    copied += 99;
    std::cout << "numbers < copied: " << (numbers < copied ? "true" : "false") << '\n';
    std::cout << "numbers <= copied: " << (numbers <= copied ? "true" : "false") << '\n';
    std::cout << "numbers != copied: " << (numbers != copied ? "true" : "false") << '\n';
    std::cout << "copied > numbers: " << (copied > numbers ? "true" : "false") << '\n';
    std::cout << "copied >= numbers: " << (copied >= numbers ? "true" : "false") << '\n';

    OrderedList descending(OrderedList::descending);
    descending += 4;
    descending += 12;
    descending += 6;
    std::cout << "Descending list: " << descending.toString() << '\n';
    descending.setComparator(OrderedList::ascending);
    std::cout << "Descending list after comparator change: " << descending.toString() << '\n';

    OrderedList moved(std::move(descending));
    std::cout << "Moved list: " << moved.toString() << '\n';

    OrderedList moveAssigned;
    moveAssigned = std::move(moved);
    std::cout << "Move-assigned list: " << moveAssigned.toString() << '\n';

    OrderedList removalDemo;
    removalDemo.insert(10);
    removalDemo.insert(20);
    removalDemo.remove(10);
    std::cout << "Separate remove() demo: " << removalDemo.toString() << '\n';

    numbers.clear();
    std::cout << "After direct clear(): " << numbers.toString() << '\n';

    assigned += 11;
    !assigned;
    std::cout << "After clear with operator!: " << assigned.toString() << '\n';
    std::cout << "numbers empty: " << (numbers.empty() ? "true" : "false") << '\n';

    try {
        numbers.at(0);
    } catch (const IP2::OrderedListException& error) {
        std::cout << "Custom exception caught: " << error.what() << '\n';
    }

    try {
        copied.remove(1234);
    } catch (const std::exception& error) {
        std::cout << "Standard exception caught: " << error.what() << '\n';
    }

    return 0;
}
