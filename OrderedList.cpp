#include "OrderedList.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace ip2 {

OrderedListException::OrderedListException(const std::string& message)
    : std::logic_error(message) {
}

class OrderedList::Implementation {
public:
    int* values;
    std::size_t size;
    std::size_t capacity;
    Comparator comparator;

    explicit Implementation(Comparator order)
        : values(nullptr), size(0), capacity(0), comparator(order) {
    }

    Implementation(const Implementation& other)
        : values(nullptr), size(other.size), capacity(other.capacity), comparator(other.comparator) {
        if (capacity > 0) {
            values = new int[capacity];
            for (std::size_t index = 0; index < size; ++index) {
                values[index] = other.values[index];
            }
        }
    }

    ~Implementation() {
        delete[] values;
    }

    void ensureCapacity(std::size_t requiredSize) {
        if (requiredSize <= capacity) {
            return;
        }

        std::size_t newCapacity = capacity == 0 ? 4 : capacity * 2;
        while (newCapacity < requiredSize) {
            newCapacity *= 2;
        }

        int* newValues = new int[newCapacity];
        for (std::size_t index = 0; index < size; ++index) {
            newValues[index] = values[index];
        }

        delete[] values;
        values = newValues;
        capacity = newCapacity;
    }

    bool areEqual(int left, int right) const {
        return !comparator(left, right) && !comparator(right, left);
    }

    std::size_t insertionIndex(int value) const {
        std::size_t left = 0;
        std::size_t right = size;

        while (left < right) {
            const std::size_t middle = left + (right - left) / 2;
            if (comparator(value, values[middle])) {
                right = middle;
            } else {
                left = middle + 1;
            }
        }

        return left;
    }

    int searchIndex(int value) const {
        std::size_t left = 0;
        std::size_t right = size;

        while (left < right) {
            const std::size_t middle = left + (right - left) / 2;

            if (areEqual(values[middle], value)) {
                std::size_t first = middle;
                while (first > 0 && areEqual(values[first - 1], value)) {
                    --first;
                }
                return static_cast<int>(first);
            }

            if (comparator(values[middle], value)) {
                left = middle + 1;
            } else {
                right = middle;
            }
        }

        return -1;
    }
};

bool OrderedList::ascending(int left, int right) noexcept {
    return left < right;
}

bool OrderedList::descending(int left, int right) noexcept {
    return left > right;
}

OrderedList::OrderedList(Comparator comparator)
    : impl_(nullptr) {
    if (comparator == nullptr) {
        throw std::invalid_argument("Comparator must not be null.");
    }

    impl_ = new Implementation(comparator);
}

OrderedList::OrderedList(const OrderedList& other)
    : impl_(new Implementation(*other.impl_)) {
}

OrderedList& OrderedList::operator=(const OrderedList& other) {
    if (this == &other) {
        return *this;
    }

    Implementation* replacement = new Implementation(*other.impl_);
    delete impl_;
    impl_ = replacement;
    return *this;
}

OrderedList::OrderedList(OrderedList&& other) noexcept
    : impl_(other.impl_) {
    other.impl_ = nullptr;
}

OrderedList& OrderedList::operator=(OrderedList&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    delete impl_;
    impl_ = other.impl_;
    other.impl_ = nullptr;
    return *this;
}

OrderedList::~OrderedList() {
    delete impl_;
}

void OrderedList::insert(int value) {
    impl_->ensureCapacity(impl_->size + 1);
    const std::size_t insertAt = impl_->insertionIndex(value);

    for (std::size_t index = impl_->size; index > insertAt; --index) {
        impl_->values[index] = impl_->values[index - 1];
    }

    impl_->values[insertAt] = value;
    ++impl_->size;
}

void OrderedList::remove(int value) {
    if (impl_->size == 0) {
        throw OrderedListException("Cannot remove from an empty ordered list.");
    }

    const int index = impl_->searchIndex(value);
    if (index < 0) {
        throw std::invalid_argument("Value was not found in the ordered list.");
    }

    for (std::size_t current = static_cast<std::size_t>(index); current + 1 < impl_->size; ++current) {
        impl_->values[current] = impl_->values[current + 1];
    }

    --impl_->size;
}

void OrderedList::updateAt(std::size_t index, int newValue) {
    if (impl_->size == 0) {
        throw OrderedListException("Cannot update an empty ordered list.");
    }

    if (index >= impl_->size) {
        throw std::out_of_range("Index is outside the ordered list.");
    }

    for (std::size_t current = index; current + 1 < impl_->size; ++current) {
        impl_->values[current] = impl_->values[current + 1];
    }
    --impl_->size;

    insert(newValue);
}

int OrderedList::at(std::size_t index) const {
    if (impl_->size == 0) {
        throw OrderedListException("Cannot read from an empty ordered list.");
    }

    if (index >= impl_->size) {
        throw std::out_of_range("Index is outside the ordered list.");
    }

    return impl_->values[index];
}

bool OrderedList::contains(int value) const noexcept {
    return find(value) >= 0;
}

int OrderedList::count(int value) const noexcept {
    const int first = find(value);
    if (first < 0) {
        return 0;
    }

    int matches = 0;
    for (std::size_t index = static_cast<std::size_t>(first); index < impl_->size; ++index) {
        if (!impl_->areEqual(impl_->values[index], value)) {
            break;
        }
        ++matches;
    }
    return matches;
}

int OrderedList::find(int value) const noexcept {
    if (impl_->size == 0) {
        return -1;
    }

    return impl_->searchIndex(value);
}

std::size_t OrderedList::size() const noexcept {
    return impl_->size;
}

bool OrderedList::empty() const noexcept {
    return impl_->size == 0;
}

void OrderedList::clear() noexcept {
    impl_->size = 0;
}

void OrderedList::setComparator(Comparator comparator) {
    if (comparator == nullptr) {
        throw std::invalid_argument("Comparator must not be null.");
    }

    impl_->comparator = comparator;
    std::sort(impl_->values, impl_->values + impl_->size, impl_->comparator);
}

OrderedList::Comparator OrderedList::getComparator() const noexcept {
    return impl_->comparator;
}

std::string OrderedList::toString() const {
    std::ostringstream builder;
    builder << "OrderedList{size=" << impl_->size
            << ", capacity=" << impl_->capacity
            << ", comparator=" << (impl_->comparator == &OrderedList::descending ? "descending" : "ascending")
            << ", values=[";

    for (std::size_t index = 0; index < impl_->size; ++index) {
        builder << impl_->values[index];
        if (index + 1 < impl_->size) {
            builder << ", ";
        }
    }

    builder << "]}";
    return builder.str();
}

OrderedList& OrderedList::operator+=(int value) {
    insert(value);
    return *this;
}

OrderedList& OrderedList::operator-=(int value) {
    remove(value);
    return *this;
}

OrderedList& OrderedList::operator*=(const EditCommand& command) {
    updateAt(command.first, command.second);
    return *this;
}

OrderedList& OrderedList::operator!() noexcept {
    clear();
    return *this;
}

int OrderedList::operator[](int value) const noexcept {
    return find(value);
}

bool OrderedList::operator==(const OrderedList& other) const noexcept {
    if (impl_->size != other.impl_->size) {
        return false;
    }

    for (std::size_t index = 0; index < impl_->size; ++index) {
        if (impl_->values[index] != other.impl_->values[index]) {
            return false;
        }
    }

    return true;
}

bool OrderedList::operator!=(const OrderedList& other) const noexcept {
    return !(*this == other);
}

bool OrderedList::operator<(const OrderedList& other) const noexcept {
    return std::lexicographical_compare(
        impl_->values,
        impl_->values + impl_->size,
        other.impl_->values,
        other.impl_->values + other.impl_->size);
}

bool OrderedList::operator<=(const OrderedList& other) const noexcept {
    return !(*this > other);
}

bool OrderedList::operator>(const OrderedList& other) const noexcept {
    return other < *this;
}

bool OrderedList::operator>=(const OrderedList& other) const noexcept {
    return !(*this < other);
}

}
