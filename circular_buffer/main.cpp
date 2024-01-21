#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <gtest/gtest.h>

typedef char value_type;

class CircularBuffer {
private:
    value_type* buffer;
    value_type* begin;
    value_type* end;
    int current_size;
    int current_capacity;


    int adjust_index(int i) const {
        return (begin - buffer + i) % current_capacity;
    }

public:
    CircularBuffer() : buffer(nullptr), begin(nullptr), end(nullptr), current_size(0), current_capacity(0) {}
    explicit CircularBuffer(int capacity) : current_size(0), current_capacity(capacity) {
        buffer = new value_type[capacity];
        begin = buffer;
        end = buffer;//+ capacity
    }
    CircularBuffer(int capacity, const value_type& elem) : CircularBuffer(capacity) {
        std::fill(buffer, buffer + capacity, elem);
        end = buffer + capacity;
        current_size = capacity;
    }

    ~CircularBuffer() {
        delete[] buffer;
    }
    CircularBuffer(const CircularBuffer& cb) : current_size(cb.current_size), current_capacity(cb.current_capacity) {
        buffer = new value_type[current_capacity];
        std::memcpy(buffer, cb.buffer, current_capacity);
        begin = buffer + (cb.begin - cb.buffer);
        end = buffer + (cb.end - cb.buffer);
    }
    value_type& operator[](int i) {
        return buffer[adjust_index(i)];
    }
    const value_type& operator[](int i) const {
        return buffer[adjust_index(i)];
    }
    value_type& at(int i) {
        if (i < 0 || i >= current_size)
            throw std::out_of_range("Index out of range");
        return (*this)[i];
    }

    const value_type& at(int i) const {
        if (i < 0 || i >= current_size)
            throw std::out_of_range("Index out of range");
        return (*this)[i];
    }

    value_type& front() {
        return *begin;
    }

    const value_type& front() const {
        return *begin;
    }

    value_type& back() {
        if (end == buffer) {
            return buffer[current_capacity - 1];
        }
        return *(end - 1);
    }

    const value_type& back() const {
        if (end == buffer) {
            return buffer[current_capacity - 1];
        }
        return *(end - 1);
    }

    value_type* linearize() {
        if (is_linearized()) {
            return buffer;
        }
        value_type* new_buffer = new value_type[current_capacity];

        int first_chunk = end - buffer;
        std::memcpy(new_buffer, begin, first_chunk);
        std::memcpy(new_buffer + first_chunk, buffer, current_size - first_chunk);

        delete[] buffer;
        buffer = new_buffer;
        begin = buffer;
        end = buffer + current_size;
        return buffer;
    }

    bool is_linearized() const {
        return begin <= end;
    }
    void rotate(int new_begin) {
        linearize();
        begin = buffer + new_begin;
    }

    int size() const {
        return current_size;
    }

    bool empty() const {
        return current_size == 0;
    }

    bool full() const {
        return current_size == current_capacity;
    }

    int reserve() const {
        return current_capacity - current_size;
    }

    int capacity() const {
        return current_capacity;
    }

    void push_back(const value_type& item = value_type()) {
        if (current_size == current_capacity) {
            *begin = item;
            begin = (begin + 1 == buffer + current_capacity) ? buffer : begin + 1;
            end = (end + 1 == buffer + current_capacity) ? buffer : end + 1;
        } else {
            *end = item;
            end = (end + 1 == buffer + current_capacity) ? buffer : end + 1;
            current_size++;
        }
    }

    void set_capacity(int new_capacity) {
        if (new_capacity < current_size)
            throw std::runtime_error("New capacity is smaller than current size");
        value_type* new_buffer = new value_type[new_capacity];
        if (is_linearized()) {
            std::memcpy(new_buffer, begin, current_size);
        } else {
            int first_chunk = buffer + current_capacity - begin;
            std::memcpy(new_buffer, begin, first_chunk);
            std::memcpy(new_buffer + first_chunk, buffer, current_size - first_chunk);
        }
        delete[] buffer;
        buffer = new_buffer;
        begin = buffer;
        end = buffer + current_size;
        current_capacity = new_capacity;
    }
    void resize(int new_size, const value_type& item = value_type()) {
        if (new_size < 0)
            throw std::runtime_error("New size cannot be zero");

        if (new_size < current_size) {
            current_size = new_size;
            end = buffer + adjust_index(new_size);
        } else if (new_size > current_size) {
            set_capacity(new_size);
            for (int i = current_size; i < new_size; ++i) {
                push_back(item);
            }
        }
    }
    CircularBuffer& operator=(const CircularBuffer& cb) {
        if (&cb == this) return *this;
        delete[] buffer;
        current_capacity = cb.current_capacity;
        current_size = cb.current_size;
        buffer = new value_type[current_capacity];
        std::memcpy(buffer, cb.buffer, current_capacity);
        begin = buffer + (cb.begin - cb.buffer);
        end = buffer + (cb.end - cb.buffer);
        return *this;
    }
    void swap(CircularBuffer& cb) {
        std::swap(buffer, cb.buffer);
        std::swap(begin, cb.begin);
        std::swap(end, cb.end);
        std::swap(current_size, cb.current_size);
        std::swap(current_capacity, cb.current_capacity);
    }

    void push_front(const value_type& item = value_type()) {
        if (full()) {
            begin = begin == buffer ? buffer + current_capacity - 1 : begin - 1;
            *begin = item;
        } else {
            begin = begin == buffer ? buffer + current_capacity - 1 : begin - 1;
            *begin = item;
            current_size++;
        }
    }

    void pop_back() {
        if (empty()) return;
        end = end == buffer ? buffer + current_capacity - 1 : end - 1;
        current_size--;
    }

    void pop_front() {
        if (empty()) return;
        begin = begin == buffer + current_capacity - 1 ? buffer : begin + 1;
        current_size--;
    }
    void insert(int pos, const value_type& item = value_type()) {
        if (pos < 0 || pos > current_size)
            throw std::out_of_range("Index out of range");
        if (full()) {
            push_front(buffer[adjust_index(current_size - 1)]);
            current_size--;
        }
        for (int i = current_size; i > pos; --i) {
            buffer[adjust_index(i)] = buffer[adjust_index(i - 1)];
        }
        buffer[adjust_index(pos)] = item;
        current_size++;
    }
    void erase(int first, int last) {
        if (first < 0 || last > current_size || first > last)
            throw std::out_of_range("Invalid range");

        for (int i = first; i < current_size - (last - first); ++i) {
            buffer[adjust_index(i)] = buffer[adjust_index(i + (last - first))];
        }
        current_size -= (last - first);
    }

    void clear() {
        current_size = 0;
        begin = buffer;
        end = buffer;
    }
};

bool operator==(const CircularBuffer& a, const CircularBuffer& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}
bool operator!=(const CircularBuffer& a, const CircularBuffer& b) {
    return !(a == b);
}

int main() {
    CircularBuffer buffer(5);
    buffer.push_back('A');
    buffer.push_back('B');
    buffer.push_back('C');
    buffer.push_back('D');
    std::cout << "Buffer after pushing A, B, C, D: ";
    for (int i = 0; i < buffer.size(); ++i) {
        std::cout << buffer[i] << " ";
    }
    std::cout << std::endl;

    buffer.pop_front();

    std::cout << "Buffer after popping the front: ";
    for (int i = 0; i < buffer.size(); ++i) {
        std::cout << buffer[i] << " ";
    }
    std::cout << std::endl;

    buffer.push_back('E');
    buffer.push_back('F');
    buffer.push_back('G');

    std::cout << "Buffer after pushing E, F, G: ";
    for (int i = 0; i < buffer.size(); ++i) {
        std::cout << buffer[i] << " ";
    }
    std::cout << std::endl;
    try {
        std::cout << "Element at index 2: " << buffer.at(2) << std::endl;
        std::cout << "Element at index 6: " << buffer.at(6) << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
//
//TEST(CircularBufferTest, DefaultConstructor) {
//    CircularBuffer buf;
//    ASSERT_TRUE(buf.empty());
//    ASSERT_EQ(buf.size(), 0);
//    ASSERT_EQ(buf.capacity(), 0);
}
//
//TEST(CircularBufferTest, ConstructorWithCapacity) {
//    CircularBuffer buf(5);
//    ASSERT_TRUE(buf.empty());
//    ASSERT_EQ(buf.size(), 0);
//    ASSERT_EQ(buf.capacity(), 5);
//}
//
//TEST(CircularBufferTest, ConstructorWithElement) {
//    CircularBuffer buf(5, 'a');
//    ASSERT_FALSE(buf.empty());
//    ASSERT_EQ(buf.size(), 5);
//    ASSERT_EQ(buf.capacity(), 5);
//    for (int i = 0; i < 5; i++) {
//        ASSERT_EQ(buf[i], 'a');
//    }
//}
//
//TEST(CircularBufferTest, PushBackAndFront) {
//    CircularBuffer buf(3);
//    buf.push_back('a');
//    ASSERT_EQ(buf.back(), 'a');
//    buf.push_front('b');
//    ASSERT_EQ(buf.front(), 'b');
//    buf.push_back('c');
//    ASSERT_EQ(buf[2], 'c');
//    ASSERT_EQ(buf.size(), 3);
//    buf.push_back('d');  // This will overwrite the 'b' at the front
//    ASSERT_EQ(buf.front(), 'a');
//}
//
//TEST(CircularBufferTest, PopBackAndFront) {
//    CircularBuffer buf(3, 'a');
//    buf.pop_back();
//    ASSERT_EQ(buf.size(), 2);
//    buf.pop_front();
//    ASSERT_EQ(buf.size(), 1);
//    ASSERT_EQ(buf.front(), 'a');
//}
//
//TEST(CircularBufferTest, InsertAndErase) {
//    CircularBuffer buf(3);
//    buf.push_back('a');
//    buf.push_back('b');
//    buf.push_back('c');
//    buf.insert(1, 'd');
//    ASSERT_EQ(buf[1], 'd');
//    ASSERT_EQ(buf.size(), 3);
//    buf.erase(1, 2);
//    ASSERT_EQ(buf[1], 'c');
//    ASSERT_EQ(buf.size(), 2);
//}
//
//TEST(CircularBufferTest, Clear) {
//    CircularBuffer buf(5, 'a');
//    buf.clear();
//    ASSERT_TRUE(buf.empty());
//    ASSERT_EQ(buf.size(), 0);
//}
//
//TEST(CircularBufferTest, LinearizeAndRotate) {
//    CircularBuffer buf(5);
//    buf.push_back('a');
//    buf.push_back('b');
//    buf.push_back('c');
//    buf.push_front('d');
//    buf.push_front('e');
//    buf.linearize();
//    ASSERT_TRUE(buf.is_linearized());
//    buf.rotate(2);
//    ASSERT_EQ(buf.front(), 'c');
//}
//
//TEST(CircularBufferTest, Resize) {
//    CircularBuffer buf(3, 'a');
//    buf.resize(5, 'b');
//    ASSERT_EQ(buf.size(), 5);
//    ASSERT_EQ(buf[3], 'b');
//    ASSERT_EQ(buf[4], 'b');
//}
//
//TEST(CircularBufferTest, Swap) {
//    CircularBuffer buf1(3, 'a');
//    CircularBuffer buf2(2, 'b');
//    buf1.swap(buf2);
//    ASSERT_EQ(buf1.size(), 2);
//    ASSERT_EQ(buf1[0], 'b');
//    ASSERT_EQ(buf2.size(), 3);
//    ASSERT_EQ(buf2[0], 'a');
//}
//
//TEST(CircularBufferTest, Equality) {
//    CircularBuffer buf1(3, 'a');
//    CircularBuffer buf2(3, 'a');
//    CircularBuffer buf3(3, 'b');
//    ASSERT_TRUE(buf1 == buf2);
//    ASSERT_FALSE(buf1 == buf3);
//    ASSERT_TRUE(buf1 != buf3);
//}
//
//int main(int argc, char **argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}


