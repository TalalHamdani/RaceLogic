# Data Structures Used in RaceLogic v0.1

This document lists all data structures employed in the project, categorizing them as **Built-in** (Language Primitives) or **Custom Made** (Implemented from scratch).

## 1. Custom Made Data Structures
These structures replaced the Standard Template Library (STL) to demonstrate low-level implementation skills.

| Structure Name | Implementation Details | Usage |
| :--- | :--- | :--- |
| **`LinkedList<T>`** | **Doubly Linked List**. Nodes contain `next` and `prev` pointers. Supports O(1) insertions/deletions at ends. | Used in `RaceGraph` to store adjacency lists (edges) efficiently. |
| **`BSTMap<K, V>`** | **Binary Search Tree**. Nodes contain Key, Value, Left, and Right pointers. Recursively ordered. | Used in `SeasonManager` and `RaceGraph` to map IDs to data (replacing `std::map`). |
| **`PriorityQueue<T>`** | **Min-Heap**. Wrapper around `std::vector<T>` that enforces heap property via `siftUp`/`siftDown`. | Used in `RaceGraph` for Dijkstra's Algorithm (finding the fastest path). |
| **`MaxHeap`** | **Max-Heap**. Specialized class using `std::vector<Driver*>`. | Used in `SeasonManager` to maintain the live race leaderboard (Drivers sorted by score). |
| **`DriverRegistry`** | **Hash Table**. Fixed-size array (`Node* table[128]`) with **Chaining** (Linked List) for collision resolution. | Used to look up `Driver` objects by their string ID string O(1) average time. |

## 2. Built-in Data Structures / Types
These are C++ language primitives used as building blocks for the custom structures.

| Structure Name | Usage |
| :--- | :--- |
| **`std::vector`** | **Dynamic Array**. replaced Custom `Vector` per user request. Used for lists, history, and heap storage. |
| **Arrays (Raw)** | Used internally in `DriverRegistry` (`Node* table[]`). |
| **Pointers** | Extensively used for relationships and memory management (`Driver*`, `Node*`). |
| **Structs / Classes** | Used to define data shapes (`Driver`, `Event`, `Result`) and encapsulate logic. |
