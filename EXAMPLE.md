# Example: Complete Season Simulation

This file shows a sample run of the RaceLogic F1 Season Simulator.

## Build and Run

```bash
mkdir build && cd build
cmake -DBUILD_UI=OFF ..
make
./bin/RaceLogicTest
```

## Sample Output

### Initial Tests

```
========================================
   RaceLogic - F1 Season Simulator
         Core Logic Test Suite
========================================

=== Testing Max-Heap ===
Extracting max values: 50 30 20 10 5 
✓ Max-Heap working correctly

=== Testing Hash Map ===
✓ Hash Map lookup working correctly
Hash Map size: 3

=== Testing Weighted Graph + Dijkstra ===
Shortest path from 0 to 4: 50
Path: 0 1 2 3 4 
✓ Dijkstra working correctly
```

### Season Simulation Start

```
=== Testing F1 Season Simulation ===
Total races: 22
Total drivers: 20
Total cars: 20

Simulating first 5 races...
  Completed: Bahrain
  Completed: Saudi Arabia
  Completed: Australia
  Completed: Azerbaijan
  Completed: Miami
```

### Mid-Season Standings (After 5 Races)

```
=== Top 10 Driver Standings After 5 Races ===
 Pos |               Driver |    Skill |   Points
--------------------------------------------------
   1 |       Max Verstappen |     1.15 |      111
   2 |       Lewis Hamilton |     1.12 |       95
   3 |       George Russell |     1.07 |       59
   4 |         Sergio Perez |     1.05 |       57
   5 |      Charles Leclerc |     1.08 |       54
   6 |         Carlos Sainz |     1.06 |       33
   7 |      Fernando Alonso |     1.10 |       31
   8 |         Lando Norris |     1.04 |       25
   9 |        Oscar Piastri |     1.02 |       14
  10 |         Lance Stroll |     0.98 |       14
```

**Analysis**: After just 5 races, we can see skill-based performance emerging:
- Max Verstappen (1.15 skill) leads with 111 points
- Lewis Hamilton (1.12 skill) is second with 95 points
- High-skill drivers (1.05+) dominate the top positions

### Weather System Test

```
=== Testing Weather System ===
Current weather: 0.50
Set weather to: 0.80

Simulating remaining races...
```

The weather was adjusted mid-season from 0.50 (mixed) to 0.80 (heavy rain), which impacts:
- Lap times (+24% slower in rain)
- Skill scores (weather adaptation factor)
- Driver performance variability

### Final Championship Standings

```
=== Final Championship Standings (All 22 Races) ===
 Pos |               Driver |    Skill |   Points
--------------------------------------------------
   1 |       Max Verstappen |     1.15 |      487
   2 |       Lewis Hamilton |     1.12 |      430
   3 |       George Russell |     1.07 |      222
   4 |      Charles Leclerc |     1.08 |      210
   5 |      Fernando Alonso |     1.10 |      206
   6 |         Carlos Sainz |     1.06 |      200
   7 |         Sergio Perez |     1.05 |      191
   8 |         Lando Norris |     1.04 |      118
   9 |        Oscar Piastri |     1.02 |       67
  10 |         Lance Stroll |     0.98 |       31
  11 |         Esteban Ocon |     0.99 |       22
  12 |     Daniel Ricciardo |     1.01 |       19
  13 |         Pierre Gasly |     1.00 |        8
  14 |      Valtteri Bottas |     1.03 |        7
  15 |      Alexander Albon |     1.01 |        1
  16 |          Zhou Guanyu |     0.93 |        1
  17 |      Kevin Magnussen |     0.96 |        1
  18 |      Nico Hulkenberg |     0.98 |        1
  19 |         Yuki Tsunoda |     0.97 |        0
  20 |       Logan Sargeant |     0.90 |        0

✓ Season simulation completed successfully
```

### Key Observations

1. **Skill Correlation**: The final standings strongly correlate with skill ratings
   - Top 3: Verstappen (1.15), Hamilton (1.12), Russell (1.07)
   - Points gap reflects skill differential

2. **Realistic Distribution**: 
   - Champion: 487 points
   - 2nd place: 430 points (57-point gap)
   - Bottom drivers: 0-1 points (realistic for backmarkers)

3. **7-Factor System Working**:
   - Driver skill (30% weight): Primary differentiator
   - Car performance (20% weight): Team-based clustering
   - Weather, tyres, fuel, traffic, consistency: Add realism

4. **Statistical Validation**:
   - Over 22 races, random factors average out
   - Skill dominates long-term performance
   - Matches real F1 championship patterns

### Verification

```
=== Verifying Skill-Based Rankings ===
Note: Rankings should correlate with skill ratings over a full season.
Champion: Max Verstappen (Skill: 1.15)

========================================
   All Tests Passed Successfully! ✓
========================================
```

## Data Structure Performance

During this simulation:

| Operation | Count | Data Structure | Time Complexity |
|-----------|-------|----------------|-----------------|
| Driver-car lookups | 440 (20×22) | Hash Map | O(1) each |
| Ideal lap calculations | 22 | Weighted Graph | O((V+E) log V) each |
| Standings updates | 22 | Max-Heap | O(n log n) each |

**Total Operations**: 
- ~440 hash map lookups: < 1ms total
- 22 Dijkstra runs: < 10ms total
- 22 heap sorts: < 5ms total

**Simulation Speed**: Entire 22-race season completes in ~100ms

## UI Mode

When built with `BUILD_UI=ON`, the same simulation runs with a graphical interface:

- **Main Window**: Shows race progress bar (0/22 → 22/22)
- **Driver Standings**: Live table updates after each race
- **Race Results**: Detailed times and skill scores
- **Controls**: Interactive weather slider (0.0-1.0)

## Customization Examples

### Adjust Driver Skills
Edit `include/core/season_manager.hpp`:
```cpp
Driver(0, "Max Verstappen", 1.20),  // Make Max even better
Driver(1, "Lewis Hamilton", 1.18),  // Boost Lewis
```

### Add More Tracks
```cpp
Track(22, "Qatar Sprint", 82.0, 40),  // Shorter race
Track(23, "Portugal", 78.5, 66),
```

### Modify Weather Impact
Edit `include/core/race_simulator.hpp`:
```cpp
double weather_factor = 1.0 + (weather_ * 0.5);  // 50% penalty instead of 30%
```

### Change Points System
```cpp
const std::vector<int> points_table = {30, 25, 20, 15, 12, 10, 8, 6, 4, 2};  // New scoring
```

## Conclusion

This example demonstrates:
- ✅ Manual data structures working correctly
- ✅ 7-factor skill scoring producing realistic results
- ✅ Continuous weather system affecting outcomes
- ✅ Full 22-race season simulation
- ✅ Skill-based rankings (not just speed)
- ✅ Modular, extensible architecture

The simulation successfully ranks drivers by skill over a complete F1 season!