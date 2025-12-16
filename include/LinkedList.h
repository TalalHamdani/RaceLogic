#ifndef CUSTOM_LINKEDLIST_H
#define CUSTOM_LINKEDLIST_H

#include <iostream>

template <typename T> class LinkedList {
private:
  struct Node {
    T data;
    Node *next;
    Node *prev;

    Node(const T &val) : data(val), next(nullptr), prev(nullptr) {}
  };

  Node *head;
  Node *tail;
  size_t size;

public:
  // Iterator Helper
  class Iterator {
  private:
    Node *current;

  public:
    Iterator(Node *node) : current(node) {}

    T &operator*() { return current->data; }

    Iterator &operator++() {
      if (current)
        current = current->next;
      return *this;
    }

    bool operator!=(const Iterator &other) { return current != other.current; }
  };

  LinkedList() : head(nullptr), tail(nullptr), size(0) {}

  ~LinkedList() { clear(); }

  void push_back(const T &value) {
    Node *newNode = new Node(value);
    if (!head) {
      head = tail = newNode;
    } else {
      tail->next = newNode;
      newNode->prev = tail;
      tail = newNode;
    }
    size++;
  }

  void clear() {
    Node *current = head;
    while (current) {
      Node *temp = current;
      current = current->next;
      delete temp;
    }
    head = tail = nullptr;
    size = 0;
  }

  bool empty() const { return size == 0; }

  Iterator begin() { return Iterator(head); }
  Iterator end() { return Iterator(nullptr); }
};

#endif // CUSTOM_LINKEDLIST_H
