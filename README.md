# RaceLogic - F1 Driver Ranking System

RaceLogic is a data-driven Formula 1 ranking system built in C++. It evaluates race dynamics, tyre degradation, pit strategies, and driver performance to calculate comprehensive driver ratings using a custom-built scoring engine.

The project features a **web-based dashboard** to visualize race results and season standings in real-time.

## 🚀 Key Features

### 🛠️ Custom Data Structures (No STL)
### 🛠️ Data Structures
The core engine uses a efficient combination of built-in and custom data structures:
- **`HashMap<K, V>`**: **Custom** Hash Table implementation (with chaining) for O(1) driver lookups.
- **`LinkedList<T>`**: **Custom** Doubly Linked List for adjacency lists in the graph.
- **`MaxHeap`**: **Custom** Heap for leaderboard management (O(1) access to leader).
- **`PriorityQueue<T>`**: **Custom** Min-Heap wrapper around `std::vector` for Dijkstra's.
- **`std::vector`**: Used for sequential data (Races, Laps) where O(1) index access is superior to Map O(log n).

### 🏆 Ranking Engine
- **Tyre Degradation Model**: Degradation logic based on stint length.
- **Scoring System**: Complex driver scoring based on speed efficiency, consistency, wet weather skill, and overtakes.
- **Race Graph**: Graph-based track representation using **Dijkstra’s Algorithm** to calculate ideal racing lines and sectoral times.

### 📊 Dashboard
- Interactive HTML/JS Dashboard.
- Displays Race Results, Lap Times, Tyre Degradation %, and Season Standings.
- Visualizes tyre health with color-coded warning systems.

## 📂 Project Structure
```
RaceLogic/
├── src/
│   └── main.cpp        # Entry point (CLI Simulation)
├── include/            # Header Files
│   ├── Driver.h        # Driver Class & State
│   ├── DriverRegistry.h# Driver Database
│   ├── HashMap.h       # Custom Hash Map (Driver Lookup)
│   ├── LinkedList.h    # Custom Doubly Linked List
│   ├── MaxHeap.h       # Custom Max Heap
│   ├── Queue.h         # Custom Priority Queue
│   ├── RaceGraph.h     # Dijkstra's Algorithm & Track Graph
│   ├── ScoringEngine.h # Driver Rating Logic
│   └── SeasonManager.h # Core Simulation & Data Parsing
├── data/               # Simulation Data
│   ├── drivers.txt     # Driver Database
│   └── race_events.txt # Compressed Race Events
├── race_data_csv/      # Data Generation Tools
│   ├── generate_drivers.py # Driver Data Generator
│   ├── generate_events.py  # Race Event Generator
│   └── *.csv           # Raw Dataset
├── dashboard/          # Web Visualization
│   ├── index.html      # Dashboard Entry
│   ├── style.css       # Styles
│   ├── app.js          # Logic
│   └── data.js         # Generated Simulation Data
├── build.bat           # Build Script
└── README.md           # Documentation
```

## ⚡ How to Run

### Prerequisites
- C++ Compiler (g++ recommended)
- Windows OS (for `build.bat`)

### Build & Run
1. Open a terminal in the project root.
2. Run the build script:
   ```sh
   ./build.bat
   ```
   This will compile the engine and run the simulation.
3. The simulation output will be displayed in the terminal.
4. **View Dashboard**: Open `dashboard/index.html` in your web browser to view the visualized results and season statistics.

## 📝 Configuration
- **Drivers**: Edit `data/drivers.txt` to add or modify drivers.
- **Race Events**: Edit `data/race_events.txt` to define lap-by-lap events.

### 🧹 Data Generation
Scale data parsing using Python scripts in `race_data_csv/`:
- **`generate_drivers.py`**: Generates `drivers.txt` from driver summaries.
- **`generate_events.py`**: Compiles `race_events.txt` from raw lap times and stint CSVs.

## 👨‍💻 Authors
- **Talal Hamdani**
- **Ahmad Jawad**
