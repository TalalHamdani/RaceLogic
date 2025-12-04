# RaceLogic Development Guide

## Architecture Overview

### Manual Data Structures

The project implements three custom data structures without relying on STL containers for core logic:

#### 1. Max-Heap (`include/core/max_heap.hpp`)
- **Purpose**: Efficient live ranking of drivers
- **Operations**: 
  - Insert: O(log n)
  - Extract Max: O(log n)
  - Peek: O(1)
- **Implementation**: Array-based binary heap with custom comparator support
- **Usage**: Used in `SeasonManager::getLiveRankings()` to provide real-time driver standings

#### 2. Hash Map (`include/core/hash_map.hpp`)
- **Purpose**: Fast lookup of driver-car assignments
- **Operations**:
  - Insert: O(1) average
  - Get: O(1) average
  - Remove: O(1) average
- **Implementation**: 
  - Chaining-based collision resolution
  - Dynamic resizing with 0.75 load factor
  - Initial capacity of 16 buckets
- **Usage**: Maps driver IDs to car IDs in season management

#### 3. Weighted Graph + Dijkstra (`include/core/weighted_graph.hpp`)
- **Purpose**: Model track layouts and compute ideal lap paths
- **Operations**:
  - Add Edge: O(1)
  - Dijkstra's Algorithm: O((V + E) log V)
- **Implementation**:
  - Adjacency list representation
  - Priority queue for optimal path finding
  - Supports both directed and bidirectional edges
- **Usage**: Each track is modeled as a graph with sectors as nodes

### Core Simulation Engine

#### Race Simulator (`include/core/race_simulator.hpp`)

The heart of the simulation, calculating performance based on 7 factors:

1. **Driver Skill Rating** (30% weight)
   - Range: 0.8 - 1.2
   - Higher = better performance
   - Example: Max Verstappen = 1.15

2. **Car Performance** (20% weight)
   - Range: 0.85 - 1.15
   - Varies by team
   - Example: Red Bull Racing = 1.15

3. **Weather Adaptation**
   - Continuous weather: 0.0 (dry) to 1.0 (rain)
   - Each driver has varying rain performance
   - Up to 30% time penalty in heavy rain

4. **Tyre Management**
   - Random degradation: 1.0 - 1.15
   - Simulates tyre wear over race distance

5. **Fuel Management**
   - Random fuel load impact: 1.0 - 1.08
   - Heavier at start, lighter at end

6. **Overtaking Skill**
   - Random variation: 0.9 - 1.1
   - Simulates traffic and overtaking ability

7. **Consistency**
   - Random variation: 0.95 - 1.05
   - Race-to-race performance variation

#### Skill Score Formula

```
ActualTime = IdealTime × DriverFactor × CarFactor × WeatherFactor × 
             TyreFactor × FuelFactor × TrafficFactor × RandomFactor

SkillScore = (IdealTime / ActualTime) × 
             (DriverSkill × 0.3) × 
             (1.0 + CarPerformance × 0.2) × 
             WeatherAdaptation × TyreManagement × 
             FuelManagement × OvertakingSkill × Consistency
```

Rankings are determined by **Skill Score**, not raw speed.

#### Season Manager (`include/core/season_manager.hpp`)

Coordinates the entire F1 season:

- **22 Races**: Complete F1 calendar
- **20 Drivers**: Full grid with realistic skill ratings
- **10 Teams**: All F1 constructors with 2 cars each
- **Points System**: FIA standard (25-18-15-12-10-8-6-4-2-1)

Functions:
- `runNextRace()`: Simulate one race
- `runAllRaces()`: Complete the season
- `resetSeason()`: Start over
- `getDriverStandings()`: Get current championship order
- `getLiveRankings()`: Get heap-based rankings

### Model Classes

#### Driver (`include/models/driver.hpp`)
- ID, Name, Skill Rating
- Season points accumulator

#### Car (`include/models/car.hpp`)
- ID, Team Name, Performance Rating

#### Track (`include/models/track.hpp`)
- ID, Name, Ideal Lap Time, Number of Laps
- Weighted graph representation of track sectors

#### RaceResult (`include/models/race_result.hpp`)
- Driver ID, Position, Actual Time
- Skill Score, Points earned

### User Interface

#### RaceLogic UI (`include/ui/race_logic_ui.hpp`)

Dear ImGui-based interface with four main windows:

1. **Main Window**: Season progress, weather indicator
2. **Driver Standings**: Live championship table with skill ratings
3. **Race Results**: Detailed results from latest race
4. **Simulation Controls**: 
   - Run Next Race button
   - Run All Races button
   - Reset Season button
   - Weather slider (0.0 - 1.0)

## Building the Project

### Standard Build (with UI)
```bash
mkdir build && cd build
cmake ..
make
./bin/RaceLogic
```

### Headless Build (no UI)
```bash
mkdir build && cd build
cmake -DBUILD_UI=OFF ..
make
./bin/RaceLogicTest
```

### Dependencies
All dependencies are automatically fetched during CMake configuration:
- GLFW 3.3.8 (window management)
- Dear ImGui 1.89.9 (UI framework)
- OpenGL 3.0+ (graphics)

## Testing

Run the test suite:
```bash
./bin/RaceLogicTest
```

Tests validate:
- Max-Heap correctness (insertion, extraction)
- Hash Map operations (insert, get, remove)
- Dijkstra's algorithm (shortest path)
- Season simulation (5 races + full season)
- Weather system (continuous 0.0-1.0)
- Skill-based rankings

## Key Features

### Modular Architecture
- **Models**: Data structures for domain objects
- **Core**: Business logic and algorithms
- **UI**: Presentation layer

### No STL Containers in Core Logic
The manual data structures (MaxHeap, HashMap, WeightedGraph) demonstrate advanced C++ concepts without relying on `std::map`, `std::set`, or `std::priority_queue` for the core simulation logic.

### Realistic F1 Simulation
- Authentic driver names and skill ratings
- Real F1 teams and car performance
- Actual track names and lap times
- FIA points system
- Multi-factor performance calculation

### Extensibility
- Easy to add more drivers/teams
- Simple to extend with new tracks
- Flexible scoring formula
- Customizable weather impact

## Example Output

```
=== Final Championship Standings (All 22 Races) ===
 Pos |               Driver |    Skill |   Points
--------------------------------------------------
   1 |       Max Verstappen |     1.15 |      484
   2 |       Lewis Hamilton |     1.12 |      419
   3 |         Sergio Perez |     1.05 |      257
   4 |      Fernando Alonso |     1.10 |      242
   5 |      Charles Leclerc |     1.08 |      223
```

Notice how the final standings generally correlate with skill ratings, demonstrating that the simulation rewards driver ability over randomness.

## Future Enhancements

Potential additions:
- DNF (Did Not Finish) simulation
- Pit stop strategy
- Safety car periods
- Qualifying sessions
- Constructor's championship
- Historical season replay
- Race pace graphs
- Telemetry visualization

## License

Educational project demonstrating C++ best practices and F1 simulation.