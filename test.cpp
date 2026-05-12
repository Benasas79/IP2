#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "OrderedList.h"

class DualLogger {
public:
    explicit DualLogger(const std::string& fileName)
        : file_(fileName) {
        if (!file_) {
            throw std::runtime_error("Failed to open log file.");
        }
    }

    template <typename Type>
    DualLogger& operator<<(const Type& value) {
        std::cout << value;
        file_ << value;
        return *this;
    }

    DualLogger& operator<<(std::ostream& (*manipulator)(std::ostream&)) {
        manipulator(std::cout);
        manipulator(file_);
        return *this;
    }

private:
    std::ofstream file_;
};

namespace {

bool expectTrue(DualLogger& log, bool condition, const std::string& testName, const std::string& detail) {
    log << testName << '\n';
    log << detail << '\n';
    log << "Observed: " << (condition ? "true" : "false") << '\n';
    log << "Result: " << (condition ? "PASS" : "FAIL") << "\n\n";
    return condition;
}

bool expectEqual(DualLogger& log, int actual, int expected, const std::string& testName, const std::string& detail) {
    const bool passed = actual == expected;
    log << testName << '\n';
    log << detail << '\n';
    log << "Expected: " << expected << ", actual: " << actual << '\n';
    log << "Result: " << (passed ? "PASS" : "FAIL") << "\n\n";
    return passed;
}

bool expectText(DualLogger& log, const std::string& actual, const std::string& expectedPart, const std::string& testName, const std::string& detail) {
    const bool passed = actual.find(expectedPart) != std::string::npos;
    log << testName << '\n';
    log << detail << '\n';
    log << "Expected fragment: " << expectedPart << '\n';
    log << "Actual text: " << actual << '\n';
    log << "Result: " << (passed ? "PASS" : "FAIL") << "\n\n";
    return passed;
}

bool expectThrowsOrdered(DualLogger& log, const std::string& testName, const std::string& detail, void (*action)()) {
    try {
        action();
    } catch (const IP2::OrderedListException& error) {
        log << testName << '\n';
        log << detail << '\n';
        log << "Caught custom exception: " << error.what() << '\n';
        log << "Result: PASS\n\n";
        return true;
    } catch (const std::exception& error) {
        log << testName << '\n';
        log << detail << '\n';
        log << "Caught wrong exception: " << error.what() << '\n';
        log << "Result: FAIL\n\n";
        return false;
    }

    log << testName << '\n';
    log << detail << '\n';
    log << "No exception was thrown.\n";
    log << "Result: FAIL\n\n";
    return false;
}

bool expectThrowsStandard(DualLogger& log, const std::string& testName, const std::string& detail, void (*action)()) {
    try {
        action();
    } catch (const IP2::OrderedListException& error) {
        log << testName << '\n';
        log << detail << '\n';
        log << "Caught custom exception instead: " << error.what() << '\n';
        log << "Result: FAIL\n\n";
        return false;
    } catch (const std::exception& error) {
        log << testName << '\n';
        log << detail << '\n';
        log << "Caught standard exception: " << error.what() << '\n';
        log << "Result: PASS\n\n";
        return true;
    }

    log << testName << '\n';
    log << detail << '\n';
    log << "No exception was thrown.\n";
    log << "Result: FAIL\n\n";
    return false;
}

void readFromEmpty() {
    IP2::OrderedList list;
    list.at(0);
}

void constructWithNullComparator() {
    IP2::OrderedList list(nullptr);
}

}

int main() {
    DualLogger log("log.txt");
    int passed = 0;
    int total = 0;

    IP2::OrderedList list;
    list += 10;
    list += 2;
    list += 6;
    list += 2;

    ++total;
    passed += expectEqual(log, list.at(0), 2, "Test 1", "Smallest element should appear first after sorted inserts.");

    ++total;
    passed += expectEqual(log, list.at(3), 10, "Test 2", "Largest element should appear last in ascending order.");

    ++total;
    passed += expectEqual(log, list[2], 0, "Test 3", "operator[] should return the first position of a duplicated value.");

    ++total;
    passed += expectEqual(log, list.count(2), 2, "Test 4", "count should report both copies of the duplicated value.");

    ++total;
    passed += expectTrue(log, list.contains(6), "Test 5", "contains should report true for an inserted value.");

    list *= IP2::OrderedList::EditCommand{1, 8};
    ++total;
    passed += expectEqual(log, list.at(2), 8, "Test 6", "Editing index 1 to 8 should reinsert it into sorted position.");

    list -= 2;
    ++total;
    passed += expectEqual(log, list.count(2), 0, "Test 7", "After the earlier edit removed one duplicate, deleting 2 should remove the final remaining copy.");

    IP2::OrderedList copy(list);
    copy += 50;
    ++total;
    passed += expectEqual(log, list.count(50), 0, "Test 8", "Deep copy should prevent later edits from affecting the original.");

    ++total;
    passed += expectTrue(log, list < copy, "Test 9", "Comparison operators should align lexicographically.");

    IP2::OrderedList descending(IP2::OrderedList::descending);
    descending += 4;
    descending += 9;
    descending += 1;
    ++total;
    passed += expectEqual(log, descending.at(0), 9, "Test 10", "Descending comparator should keep the biggest value first.");

    descending.setComparator(IP2::OrderedList::ascending);
    ++total;
    passed += expectEqual(log, descending.at(0), 1, "Test 11", "Changing comparator should reorder the stored values.");

    ++total;
    passed += expectText(log, descending.toString(), "comparator=ascending", "Test 12", "toString should describe the active comparator.");

    !descending;
    ++total;
    passed += expectTrue(log, descending.empty(), "Test 13", "operator! should clear the entire list.");

    ++total;
    passed += expectThrowsOrdered(log, "Test 14", "Reading from an empty list should raise the custom exception.", &readFromEmpty);

    ++total;
    passed += expectThrowsStandard(log, "Test 15", "Constructing with a null comparator should raise a standard exception.", &constructWithNullComparator);

    log << "Summary\n";
    log << "Passed tests: " << passed << '\n';
    log << "Total tests: " << total << '\n';
    log << "Overall result: " << (passed == total ? "PASS" : "FAIL") << '\n';

    return passed == total ? 0 : 1;
}
