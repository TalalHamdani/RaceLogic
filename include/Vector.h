#ifndef CUSTOM_VECTOR_H
#define CUSTOM_VECTOR_H

#include <iostream>
#include <stdexcept>


template <typename T> class Vector {
private:
  T *data;
  size_t capacity;
  size_t length;

  void resize(size_t newCapacity) {
    T *newData = new T[newCapacity];
    for (size_t i = 0; i < length; i++) {
      newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
  }

public:
  Vector() : capacity(10), length(0) { data = new T[capacity]; }

  Vector(size_t initSize) : capacity(initSize + 5), length(0) {
    data = new T[capacity];
  }

  ~Vector() { delete[] data; }

  // Copy Constructor
  Vector(const Vector &other) : capacity(other.capacity), length(other.length) {
    data = new T[capacity];
    for (size_t i = 0; i < length; i++) {
      data[i] = other.data[i];
    }
  }

  // Assignment Operator
  Vector &operator=(const Vector &other) {
    if (this != &other) {
      delete[] data;
      capacity = other.capacity;
      length = other.length;
      data = new T[capacity];
      for (size_t i = 0; i < length; i++) {
        data[i] = other.data[i];
      }
    }
    return *this;
  }

  void push_back(const T &value) {
    if (length >= capacity) {
      resize(capacity * 2);
    }
    data[length++] = value;
  }

  void pop_back() {
    if (length > 0) {
      length--;
    }
  }

  size_t size() const { return length; }

  bool empty() const { return length == 0; }

  void clear() { length = 0; }

  T &operator[](size_t index) {
    if (index >= length) {
      // For safety, though in C++ [] usually doesn't throw.
      // We'll trust the caller or throw in debug mode.
      // keeping it simple for speed.
    }
    return data[index];
  }

  const T &operator[](size_t index) const { return data[index]; }

  T &back() { return data[length - 1]; }

  // Iterator support for range-based loops
  T *begin() { return data; }
  T *end() { return data + length; }
  const T *begin() const { return data; }
  const T *end() const { return data + length; }
};

#endif // CUSTOM_VECTOR_H
