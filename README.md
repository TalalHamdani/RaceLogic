# RaceLogic - F1 Season Simulator

A comprehensive Formula 1 season simulator built with C++ and Dear ImGui. This project simulates an entire F1 season with 20+ races, ranking drivers by skill score using advanced data structures and realistic racing factors.

## Features

### Core Simulation
- **20+ Race Season**: Complete F1 calendar with 22 authentic tracks
- **20 Drivers**: Real F1 driver lineup with individual skill ratings
- **10 Teams**: All F1 constructors with unique car performance ratings
- **7-Factor Skill Scoring System**:
  1. Driver Skill Rating (30% weight)
  2. Car Performance (20% weight)
  3. Weather Adaptation
  4. Tyre Management
  5. Fuel Management
  6. Overtaking Skill
  7. Race Consistency

### Manual Data Structures (No STL Containers Used for Core Logic)
- **Max-Heap**: Live driver rankings by points
- **Hash Map**: Driver and car registry with O(1) lookup
- **Weighted Graph + Dijkstra**: Track modeling with ideal lap calculation

### Advanced Features
- **Continuous Weather System**: 0.0 (dry) to 1.0 (heavy rain) with real-time impact
- **Season Points Tracking**: FIA-standard points system (25-18-15-12-10-8-6-4-2-1)
- **Skill Score vs Speed**: Rankings based on skill performance, not just raw speed
- **Dynamic Race Simulation**: Multiple factors affecting each lap

## Project Structure

```
RaceLogic/
├── include/
│   ├── models/          # Data models
│   │   ├── driver.hpp
│   │   ├── car.hpp
│   │   ├── track.hpp
│   │   └── race_result.hpp
│   ├── core/            # Core logic and data structures
│   │   ├── max_heap.hpp
│   │   ├── hash_map.hpp
│   │   ├── weighted_graph.hpp
│   │   ├── race_simulator.hpp
│   │   └── season_manager.hpp
│   └── ui/              # User interface
│       └── race_logic_ui.hpp
├── src/
│   └── main.cpp
├── external/            # External dependencies (auto-downloaded)
│   └── imgui/
└── CMakeLists.txt
```

## Building

### Prerequisites
- CMake 3.15 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- OpenGL 3.0+
- Internet connection (for first build to download dependencies)

### Build Instructions

#### Linux/macOS
```bash
mkdir build
cd build
cmake ..
make
./bin/RaceLogic
```

#### Windows (Visual Studio)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\bin\Release\RaceLogic.exe
```

## How to Use

### UI Components

1. **Main Window**: Shows race progress and weather conditions
2. **Driver Championship Standings**: Real-time rankings using max-heap
3. **Latest Race Results**: Detailed results with skill scores and times
4. **Simulation Controls**: 
   - Run Next Race: Simulate one race at a time
   - Run All Races: Complete the entire season
   - Reset Season: Start over
   - Weather Slider: Adjust continuous weather (0.0 = dry, 1.0 = rain)

### Running a Season

1. Launch the application
2. Adjust weather conditions if desired
3. Click "Run Next Race" to simulate races one by one, or
4. Click "Run All Races" to complete the entire season
5. View standings and results in real-time
6. Click "Reset Season" to start over

## Technical Details

### Skill Score Calculation

The skill score is calculated using:
```
Score = (Ideal Time / Actual Time) × Skill Factors
```

Where Skill Factors include:
- Driver base skill (0.8 - 1.2 range)
- Car performance (0.85 - 1.15 range)
- Weather adaptation
- Tyre degradation management
- Fuel load handling
- Overtaking ability
- Race-to-race consistency

### Data Structures

#### Max-Heap
- Custom implementation for O(log n) insertions
- Used for efficient live ranking updates
- Comparator-based for flexible sorting

#### Hash Map
- Chaining-based collision resolution
- Dynamic resizing with 0.75 load factor
- O(1) average case lookup

#### Weighted Graph
- Adjacency list representation
- Dijkstra's algorithm for optimal lap path
- Track sectors modeled as graph nodes

## Dependencies

All dependencies are automatically downloaded during CMake configuration:
- **GLFW 3.3.8**: Window management
- **Dear ImGui 1.89.9**: Immediate mode GUI
- **OpenGL 3.0+**: Graphics rendering

## License

This is an educational project demonstrating advanced C++ concepts and F1 simulation.