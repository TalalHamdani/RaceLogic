#ifndef DRIVER_REGISTRY_H
#define DRIVER_REGISTRY_H

#include "Driver.h"
#include <iostream>
#include <string>

const int TABLE_SIZE =
    128; // Small size for demo, but enough for F1 grid (20 drivers)

class DriverRegistry {
private:
  struct Node {
    Driver *driver;
    Node *next;

    Node(Driver *d) : driver(d), next(nullptr) {}
  };

  Node *table[TABLE_SIZE];

  // Simple Hash Function for strings
  int hashFunction(const std::string &key) {
    unsigned long hash = 5381;
    for (char c : key) {
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % TABLE_SIZE;
  }

public:
  DriverRegistry() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
      table[i] = nullptr;
    }
  }

  ~DriverRegistry() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
      Node *current = table[i];
      while (current != nullptr) {
        Node *temp = current;
        current = current->next;
        delete temp->driver; // Assuming Registry owns the Driver objects
        delete temp;
      }
    }
  }

  void addDriver(Driver *driver) {
    int index = hashFunction(driver->getId());
    Node *newNode = new Node(driver);

    if (table[index] == nullptr) {
      table[index] = newNode;
    } else {
      // Collision: Insert at head (Chaining)
      newNode->next = table[index];
      table[index] = newNode;
    }
  }

  Driver *getDriver(const std::string &id) {
    int index = hashFunction(id);
    Node *current = table[index];

    while (current != nullptr) {
      if (current->driver->getId() == id) {
        return current->driver;
      }
      current = current->next;
    }
    return nullptr;
  }

  // Helper to iterate (for debugging/display)
  template <typename Func> void forEach(Func func) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
      Node *current = table[i];
      while (current != nullptr) {
        func(current->driver);
        current = current->next;
      }
    }
  }
};

#endif // DRIVER_REGISTRY_H
