#ifndef CUSTOM_QUEUE_H
#define CUSTOM_QUEUE_H

#include <vector>

// Simple Min-Priority Queue wrapper around Vector
// To fully replace std::priority_queue needed for Dijkstra
template <typename T> class PriorityQueue {
private:
  std::vector<T> heap;

  int parent(int i) { return (i - 1) / 2; }
  int leftChild(int i) { return (2 * i) + 1; }
  int rightChild(int i) { return (2 * i) + 2; }

  void siftUp(int i) {
    while (i > 0 && heap[i] < heap[parent(i)]) { // Min Heap
      T temp = heap[i];
      heap[i] = heap[parent(i)];
      heap[parent(i)] = temp;
      i = parent(i);
    }
  }

  void siftDown(int i) {
    int minIndex = i;
    int l = leftChild(i);
    int r = rightChild(i);

    if (l < (int)heap.size() && heap[l] < heap[minIndex])
      minIndex = l;
    if (r < (int)heap.size() && heap[r] < heap[minIndex])
      minIndex = r;

    if (i != minIndex) {
      T temp = heap[i];
      heap[i] = heap[minIndex];
      heap[minIndex] = temp;
      siftDown(minIndex);
    }
  }

public:
  bool empty() const { return heap.empty(); }

  void push(const T &val) {
    heap.push_back(val);
    siftUp(heap.size() - 1);
  }

  T top() { return heap[0]; }

  void pop() {
    if (heap.empty())
      return;
    heap[0] = heap.back();
    heap.pop_back();
    if (!heap.empty())
      siftDown(0);
  }
};

#endif // CUSTOM_QUEUE_H
