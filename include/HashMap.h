#ifndef HASHMAP_H
#define HASHMAP_H

#include <iostream>
#include <string>
#include <vector>

// Simple Hash Node
template <typename K, typename V> struct HashNode {
  K key;
  V value;
  HashNode *next;

  HashNode(K k, V v) : key(k), value(v), next(nullptr) {}
};

// Custom Hash Map Implementation
template <typename K, typename V> class HashMap {
private:
  std::vector<HashNode<K, V> *> table;
  int capacity;
  int size;

  // Simple Hash Function
  int hashFunction(const K &key) const {
    // Handle int keys directly
    if constexpr (std::is_same<K, int>::value) {
      return key % capacity;
    }
    // Handle std::string keys
    else {
      unsigned long hash = 5381;
      for (char c : key) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
      }
      return hash % capacity;
    }
  }

public:
  HashMap(int cap = 100) : capacity(cap), size(0) {
    table.resize(capacity, nullptr);
  }

  // Destructor
  ~HashMap() { clear(); }

  // Deep Copy Constructor
  HashMap(const HashMap &other) : capacity(other.capacity), size(other.size) {
    table.resize(capacity, nullptr);
    for (int i = 0; i < capacity; ++i) {
      if (other.table[i]) {
        // Deep copy the chain
        table[i] =
            new HashNode<K, V>(other.table[i]->key, other.table[i]->value);
        HashNode<K, V> *src = other.table[i]->next;
        HashNode<K, V> *dest = table[i];
        while (src) {
          dest->next = new HashNode<K, V>(src->key, src->value);
          dest = dest->next;
          src = src->next;
        }
      }
    }
  }

  // Assignment Operator
  HashMap &operator=(const HashMap &other) {
    if (this != &other) {
      clear();
      capacity = other.capacity;
      size = other.size;
      table.resize(capacity, nullptr);
      for (int i = 0; i < capacity; ++i) {
        if (other.table[i]) {
          table[i] =
              new HashNode<K, V>(other.table[i]->key, other.table[i]->value);
          HashNode<K, V> *src = other.table[i]->next;
          HashNode<K, V> *dest = table[i];
          while (src) {
            dest->next = new HashNode<K, V>(src->key, src->value);
            dest = dest->next;
            src = src->next;
          }
        }
      }
    }
    return *this;
  }

  // Insert or Update
  void put(const K &key, const V &value) {
    int hashIndex = hashFunction(key);
    HashNode<K, V> *prev = nullptr;
    HashNode<K, V> *entry = table[hashIndex];

    while (entry != nullptr) {
      if (entry->key == key) {
        entry->value = value;
        return;
      }
      prev = entry;
      entry = entry->next;
    }

    HashNode<K, V> *newNode = new HashNode<K, V>(key, value);
    if (prev == nullptr) {
      table[hashIndex] = newNode;
    } else {
      prev->next = newNode;
    }
    size++;
  }

  // Get Value (Returns reference, creates default if missing like std::map)
  V &operator[](const K &key) {
    int hashIndex = hashFunction(key);
    HashNode<K, V> *prev = nullptr;
    HashNode<K, V> *entry = table[hashIndex];

    while (entry != nullptr) {
      if (entry->key == key) {
        return entry->value;
      }
      prev = entry;
      entry = entry->next;
    }

    // Key not found, insert default
    V defaultValue = V();
    HashNode<K, V> *newNode = new HashNode<K, V>(key, defaultValue);
    if (prev == nullptr) {
      table[hashIndex] = newNode;
    } else {
      prev->next = newNode;
    }
    size++;
    return newNode->value;
  }

  // Check if key exists
  bool contains(const K &key) const {
    int hashIndex = hashFunction(key);
    HashNode<K, V> *entry = table[hashIndex];
    while (entry != nullptr) {
      if (entry->key == key) {
        return true;
      }
      entry = entry->next;
    }
    return false;
  }

  // Get Size
  int getSize() const { return size; }

  // Clear map
  void clear() {
    for (int i = 0; i < capacity; ++i) {
      HashNode<K, V> *entry = table[i];
      while (entry != nullptr) {
        HashNode<K, V> *prev = entry;
        entry = entry->next;
        delete prev;
      }
      table[i] = nullptr;
    }
    size = 0;
  }
  // Iterate over all elements
  template <typename Func> void forEach(Func func) {
    for (int i = 0; i < capacity; ++i) {
      HashNode<K, V> *entry = table[i];
      while (entry != nullptr) {
        func(entry->key, entry->value);
        entry = entry->next;
      }
    }
  }
};

#endif // HASHMAP_H
