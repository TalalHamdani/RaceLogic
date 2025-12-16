#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include "Driver.h"
#include "Vector.h"
#include <iostream>

class MaxHeap {
private:
  Vector<Driver *> heap;

  int parent(int i) { return (i - 1) / 2; }
  int leftChild(int i) { return (2 * i) + 1; }
  int rightChild(int i) { return (2 * i) + 2; }

  void siftUp(int i) {
    while (i > 0 &&
           heap[parent(i)]->getRankingScore() < heap[i]->getRankingScore()) {
      std::swap(heap[i], heap[parent(i)]);
      i = parent(i);
    }
  }

  void siftDown(int i) {
    int maxIndex = i;
    int l = leftChild(i);
    int r = rightChild(i);

    if (l < heap.size() &&
        heap[l]->getRankingScore() > heap[maxIndex]->getRankingScore())
      maxIndex = l;

    if (r < heap.size() &&
        heap[r]->getRankingScore() > heap[maxIndex]->getRankingScore())
      maxIndex = r;

    if (i != maxIndex) {
      std::swap(heap[i], heap[maxIndex]);
      siftDown(maxIndex);
    }
  }

public:
  MaxHeap() {}

  void push(Driver *driver) {
    heap.push_back(driver);
    siftUp(heap.size() - 1);
  }

  Driver *pop() {
    if (heap.empty())
      return nullptr;

    Driver *root = heap[0];
    heap[0] = heap.back();
    heap.pop_back();

    if (!heap.empty()) {
      siftDown(0);
    }
    return root;
  }

  Driver *peek() {
    if (heap.empty())
      return nullptr;
    return heap[0];
  }

  bool isEmpty() { return heap.empty(); }

  int size() { return heap.size(); }

  // For live updates: Rebuilds the heap if scores change externally
  void rebuild() {
    // Start from the last non-leaf node and sift down
    for (int i = (heap.size() / 2) - 1; i >= 0; i--) {
      siftDown(i);
    }
  }

  const Vector<Driver *> &getUnderlyingContainer() const { return heap; }
};

#endif // MAX_HEAP_H
