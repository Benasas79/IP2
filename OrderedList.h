#ifndef IP2_ORDERED_LIST_H
#define IP2_ORDERED_LIST_H

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>

namespace IP2 {

class OrderedListException : public std::logic_error {
public:
    explicit OrderedListException(const std::string& message);
};

class OrderedList {
private:
    class Implementation;
    Implementation* impl_;

public:
    using Comparator = bool (*)(int left, int right);
    using EditCommand = std::pair<std::size_t, int>;

    explicit OrderedList(Comparator comparator = ascending);
    OrderedList(const OrderedList& other);
    OrderedList& operator=(const OrderedList& other);
    OrderedList(OrderedList&& other) noexcept;
    OrderedList& operator=(OrderedList&& other) noexcept;
    ~OrderedList();

    void insert(int value);
    void remove(int value);
    void updateAt(std::size_t index, int newValue);
    int at(std::size_t index) const;
    bool contains(int value) const noexcept;
    int count(int value) const noexcept;
    int find(int value) const noexcept;
    std::size_t size() const noexcept;
    bool empty() const noexcept;
    void clear() noexcept;
    void setComparator(Comparator comparator);
    Comparator getComparator() const noexcept;
    std::string toString() const;

    OrderedList& operator+=(int value);
    OrderedList& operator-=(int value);
    OrderedList& operator*=(const EditCommand& command);
    OrderedList& operator!() noexcept;
    int operator[](int value) const noexcept;

    bool operator==(const OrderedList& other) const noexcept;
    bool operator!=(const OrderedList& other) const noexcept;
    bool operator<(const OrderedList& other) const noexcept;
    bool operator<=(const OrderedList& other) const noexcept;
    bool operator>(const OrderedList& other) const noexcept;
    bool operator>=(const OrderedList& other) const noexcept;

    static bool ascending(int left, int right) noexcept;
    static bool descending(int left, int right) noexcept;
};

}

#endif
