# RaceLogic v0.1 - F1 Simulation Engine

RaceLogic is a high-performance, data-driven Formula 1 race simulation engine built in C++. It simulates race dynamics, tyre degradation, pit strategies, and driver performance using a custom-built physics and scoring engine.

The project features a **web-based dashboard** to visualize race results and season standings in real-time.

## 🚀 Key Features

### 🛠️ Custom Data Structures (No STL)
To demonstrate deep understanding of memory management and algorithm optimization, the core engine relies entirely on custom-implemented data structures instead of the C++ Standard Template Library (STL):
- **`Vector<T>`**: Dynamic array implementation with manual memory management.
- **`LinkedList<T>`**: Doubly linked list for efficient insertion/deletion.
- **`BSTMap<K, V>`**: Binary Search Tree based map for O(log n) lookups.
- **`PriorityQueue<T>`**: Min-Heap implementation for efficient Dijkstra's algorithm execution.

### 🏎️ Simulation Engine
- **Tyre Degradation Model**: Non-linear degradation logic based on tyre compound (Soft/Medium/Hard) and stint length.
- **Scoring System**: Complex driver scoring based on speed efficiency, consistency, wet weather skill, and overtakes.
- **Race Graph**: Graph-based track representation using **Dijkstra’s Algorithm** (via custom Priority Queue) to calculate ideal racing lines and sectoral times.

### 📊 Dashboard
- Interactive HTML/JS Dashboard.
- Displays Race Results, Lap Times, Tyre Degradation %, and Season Standings.
- Visualizes tyre health with color-coded warning systems.

## 📂 Project Structure

```
RaceLogic/
├── src/                # C++ Source Files (main.cpp)
├── include/            # Header Files (Custom DS, Logic)
│   ├── Vector.h        # Custom Vector
│   ├── LinkedList.h    # Custom Linked List
│   ├── BSTMap.h        # Custom BST Map
│   ├── Queue.h         # Custom Priority Queue
│   └── SeasonManager.h # Core Simulation Logic
├── data/               # Simulation Data (Drivers, Race Events)
├── dashboard/          # Web Dashboard (HTML/CSS/JS)
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
- **Race Events**: Edit `data/race_events.txt` to define lap-by-lap events using the `BATCH` format for compression.

## 👨‍💻 Author
Talal Hamdani
Formula 1 Enthusiast & Software Engineer
