#pragma once

#include <vector>
#include <functional>
#include <stdexcept>

template<typename T>
class MaxHeap {
public:
    using Comparator = std::function<bool(const T&, const T&)>;

    MaxHeap() : comparator_([](const T& a, const T& b) { return a > b; }) {}
    explicit MaxHeap(Comparator comp) : comparator_(comp) {}

    void insert(const T& value) {
        heap_.push_back(value);
        heapifyUp(heap_.size() - 1);
    }

    T extractMax() {
        if (isEmpty()) {
            throw std::runtime_error("Heap is empty");
        }
        
        T maxValue = heap_[0];
        heap_[0] = heap_.back();
        heap_.pop_back();
        
        if (!isEmpty()) {
            heapifyDown(0);
        }
        
        return maxValue;
    }

    const T& peek() const {
        if (isEmpty()) {
            throw std::runtime_error("Heap is empty");
        }
        return heap_[0];
    }

    bool isEmpty() const {
        return heap_.empty();
    }

    size_t size() const {
        return heap_.size();
    }

    void clear() {
        heap_.clear();
    }

    const std::vector<T>& getHeap() const {
        return heap_;
    }

private:
    std::vector<T> heap_;
    Comparator comparator_;

    void heapifyUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (comparator_(heap_[index], heap_[parent])) {
                std::swap(heap_[index], heap_[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(size_t index) {
        size_t size = heap_.size();
        while (true) {
            size_t largest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;

            if (left < size && comparator_(heap_[left], heap_[largest])) {
                largest = left;
            }
            if (right < size && comparator_(heap_[right], heap_[largest])) {
                largest = right;
            }

            if (largest != index) {
                std::swap(heap_[index], heap_[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }
};
