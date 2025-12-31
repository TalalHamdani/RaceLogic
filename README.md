# RaceLogic - F1 Scoring & Ranking Engine

RaceLogic is a high-performance **Scoring and Ranking Engine** for Formula 1. It processes raw race events (Lap Times, Pit Stops, Overtakes) to calculate real-time driver rankings and performance scores.

Unlike a physics simulation, RaceLogic focuses on **Data Processing** and **Algorithmic Ranking** using efficient custom data structures.

## 🚀 Key Features

### 🛠️ Data Structures & Algorithms
The engine is architected to optimize for specific operations:

*   **`HashMap<K, V>`**: **Custom** Hash Table implementation (with chaining) for **O(1) Driver Lookups**. Essential for processing high-frequency lap events.
*   **`MaxHeap`**: **Custom** Binary Heap for the **Live Leaderboard**. Allows O(log N) re-ranking and O(1) peek at the race leader.

*   **`std::vector`**: Used for linear data storage (Event Streams, History) where contiguous memory cache efficiency outweighs the need for complex localized inserts.

### 🏆 Scoring Engine
The core logic (`ScoringEngine.h`) evaluates drivers on multiple dimensions:
*   **Speed Efficiency**: Comparison against the track's Ideal Lap Time.
*   **Tyre Management**: Non-linear degradation penalties based on stint length and compound.
*   **Consistency**: Bonuses for maintaining steady pace variance < 0.1s.
*   **Aggression**: Points for confirmed overtakes.

### 📊 Web Dashboard
*   **JSON Integration**: The engine exports `window.raceData` to `dashboard/data.js`.
*   **Features**:
    *   Season Standings Table
    *   Race-by-Race Result Views
    *   Visual "Gap to Leader" and Tyre Health indicators.

## 📂 Project Structure

```
RaceLogic/
├── src/
│   └── main.cpp            # Engine Entry Point (Event Loop)
├── include/                # Header Files
│   ├── Driver.h            # Driver Entity
│   ├── DriverRegistry.h    # HashMap Wrapper (Database)
│   ├── HashMap.h           # Custom Hash Map Implementation
│   ├── MaxHeap.h           # Custom Priority Queue (Leaderboard)
│   ├── ScoringEngine.h     # Static Logic Class for Points/Score
│   └── SeasonManager.h     # Central "Brain" (Process Logic)
├── data/                   # Configuration & Inputs
│   ├── drivers.txt         # Driver Roster
│   ├── race_events.txt     # The "Tape" of race events
│   └── ideal_lap_times.txt # Track Reference Data
├── dashboard/              # Visualization (No Server Required)
│   ├── index.html
│   ├── app.js
│   └── data.js             # Output Artifact
└── build.bat               # Build Script
```

## ⚡ How to Run

### Prerequisites
- C++ Compiler (G++ via MinGW recommended)
- Windows (for `build.bat`)

### Build & Execute
1. Open a terminal in the project root.
2. Run the build script:
   ```powershell
   ./build.bat
   ```
3. The engine will process the season and output text logs to the console.
4. **Visualize**: Open `dashboard/index.html` in any web browser to see the interactive results.

## 📝 Configuration
*   **Ideal Times**: Modify `data/ideal_lap_times.txt` to adjust the "Par Time" for tracks (e.g., `Bahrain,91.5`).
*   **Drivers**: Add new drivers to `data/drivers.txt`.

## 👨‍💻 Authors
- **Talal Hamdani**
- **Ahmad Jawad**
