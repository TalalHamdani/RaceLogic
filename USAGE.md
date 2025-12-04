# Data Structure Usage Examples

This document demonstrates how the three manual data structures are used in RaceLogic.

## 1. Max-Heap Usage

### File: `include/core/season_manager.hpp`

The Max-Heap is used for live driver rankings:

```cpp
std::vector<Driver> getLiveRankings() const {
    // Create a max-heap with custom comparator
    MaxHeap<Driver> heap([](const Driver& a, const Driver& b) {
        return a.getSeasonPoints() > b.getSeasonPoints();
    });

    // Insert all drivers
    for (const auto& driver : drivers_) {
        heap.insert(driver);
    }

    // Extract in sorted order
    std::vector<Driver> rankings;
    while (!heap.isEmpty()) {
        rankings.push_back(heap.extractMax());
    }

    return rankings;
}
```

**Why Max-Heap?**
- Efficient O(log n) insertion as races complete
- O(log n) extraction for top-k queries
- Perfect for live leaderboards that update frequently

## 2. Hash Map Usage

### File: `include/core/season_manager.hpp`

The Hash Map maintains driver-to-car assignments:

```cpp
private:
    HashMap<int, int> driver_car_map_;  // driver_id -> car_id

void assignDriversToCars() {
    // O(1) insertion for each driver-car pair
    for (size_t i = 0; i < drivers_.size() && i < cars_.size(); ++i) {
        driver_car_map_.insert(drivers_[i].getId(), cars_[i].getId());
    }
}

void runNextRace() {
    // Prepare driver-car pairs with O(1) lookup
    std::vector<RaceSimulator::DriverCarPair> entries;
    for (auto& driver : drivers_) {
        auto car_opt = driver_car_map_.get(driver.getId());
        if (car_opt.has_value()) {
            int car_id = car_opt.value();
            entries.push_back(RaceSimulator::DriverCarPair(&driver, &cars_[car_id]));
        }
    }
    // ... simulate race
}
```

**Why Hash Map?**
- O(1) average lookup for driver-car associations
- Dynamic resizing maintains performance as grid changes
- Perfect for registry/mapping use cases

## 3. Weighted Graph + Dijkstra Usage

### File: `include/models/track.hpp`

Each track is modeled as a weighted graph:

```cpp
void buildTrackGraph() {
    // Create graph with 10 sectors per lap
    int sectors_per_lap = 10;
    track_graph_ = std::make_unique<WeightedGraph>(sectors_per_lap);
    
    // Add weighted edges representing sector times
    double sector_time = ideal_lap_time_ / sectors_per_lap;
    for (int i = 0; i < sectors_per_lap - 1; ++i) {
        track_graph_->addEdge(i, i + 1, sector_time);
    }
    // Close the loop (finish line to start)
    track_graph_->addEdge(sectors_per_lap - 1, 0, sector_time);
}
```

The graph can be queried for optimal paths:

```cpp
const WeightedGraph* graph = track.getTrackGraph();
auto result = graph->dijkstra(0, 9);  // Start sector to last sector
std::cout << "Ideal sector time: " << result.total_distance << std::endl;
```

**Why Weighted Graph?**
- Models complex track layouts naturally
- Dijkstra finds optimal racing lines
- Extensible to multi-path scenarios (pit lane, DRS zones)

## Real-World Data Flow

### Scenario: Simulating a Race

1. **Setup** (Hash Map)
   ```
   Season Manager creates driver_car_map
   → O(1) lookups to pair drivers with cars
   ```

2. **Race Simulation** (Graph)
   ```
   For each driver:
     → Get assigned car via HashMap.get()
     → Calculate ideal time using Track's WeightedGraph
     → Apply 7-factor scoring formula
   ```

3. **Results Processing** (Max-Heap)
   ```
   After race:
     → Update driver points
     → Insert all drivers into MaxHeap
     → Extract top drivers for podium/standings
   ```

### Performance Characteristics

| Operation | Data Structure | Complexity | Frequency |
|-----------|---------------|------------|-----------|
| Assign driver to car | Hash Map | O(1) | Once per season |
| Lookup car for driver | Hash Map | O(1) | 20 times per race |
| Calculate ideal lap | Graph + Dijkstra | O((V+E) log V) | Once per track |
| Update standings | Max-Heap | O(n log n) | After each race |
| Get top 10 drivers | Max-Heap | O(10 log n) | On demand |

## Advanced Usage

### Custom Comparators with Max-Heap

```cpp
// Sort by points (descending)
MaxHeap<Driver> byPoints([](const Driver& a, const Driver& b) {
    return a.getSeasonPoints() > b.getSeasonPoints();
});

// Sort by skill rating (descending)
MaxHeap<Driver> bySkill([](const Driver& a, const Driver& b) {
    return a.getSkillRating() > b.getSkillRating();
});
```

### Hash Map Iteration

```cpp
HashMap<int, int> map;
// ... insert data

// Get all driver IDs
auto driver_ids = map.keys();

// Get all car IDs
auto car_ids = map.values();

// Process each mapping
for (int driver_id : driver_ids) {
    auto car_id_opt = map.get(driver_id);
    if (car_id_opt.has_value()) {
        // Process driver-car pair
    }
}
```

### Graph Path Analysis

```cpp
WeightedGraph track(10);
// ... build track graph

// Find shortest path from start to sector 5
auto result = track.dijkstra(0, 5);

std::cout << "Distance: " << result.total_distance << std::endl;
std::cout << "Path: ";
for (int node : result.path) {
    std::cout << node << " -> ";
}
std::cout << "END" << std::endl;
```

## Testing the Data Structures

See `src/test_core.cpp` for comprehensive examples:

```cpp
void testMaxHeap() {
    MaxHeap<int> heap;
    heap.insert(10);
    heap.insert(30);
    heap.insert(20);
    // Extracts: 30, 20, 10
}

void testHashMap() {
    HashMap<std::string, int> map;
    map.insert("Driver1", 100);
    auto val = map.get("Driver1");  // Returns optional<100>
}

void testWeightedGraph() {
    WeightedGraph graph(5);
    graph.addBidirectionalEdge(0, 1, 10.0);
    auto result = graph.dijkstra(0, 4);  // Finds shortest path
}
```

## Memory Management

All data structures handle their own memory:
- **Max-Heap**: Uses `std::vector` internally, automatic cleanup
- **Hash Map**: Uses `std::list` for chaining, automatic cleanup
- **Weighted Graph**: Uses `std::vector<std::vector<Edge>>`, automatic cleanup

The Track class uses `std::unique_ptr<WeightedGraph>` for RAII:
```cpp
std::unique_ptr<WeightedGraph> track_graph_;  // Automatically deleted
```

## Summary

The three manual data structures work together to create an efficient, realistic F1 simulation:

- **Max-Heap**: Live rankings and leaderboards
- **Hash Map**: Fast driver-car lookups
- **Weighted Graph**: Track modeling and optimal paths

Each structure is chosen for its specific performance characteristics and use case, demonstrating real-world algorithm selection in C++ applications.