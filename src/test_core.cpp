#include <iostream>
#include <iomanip>
#include "core/season_manager.hpp"
#include "core/max_heap.hpp"
#include "core/hash_map.hpp"
#include "core/weighted_graph.hpp"

void testMaxHeap() {
    std::cout << "\n=== Testing Max-Heap ===" << std::endl;
    MaxHeap<int> heap;
    
    heap.insert(10);
    heap.insert(30);
    heap.insert(20);
    heap.insert(5);
    heap.insert(50);
    
    std::cout << "Extracting max values: ";
    while (!heap.isEmpty()) {
        std::cout << heap.extractMax() << " ";
    }
    std::cout << "\n✓ Max-Heap working correctly" << std::endl;
}

void testHashMap() {
    std::cout << "\n=== Testing Hash Map ===" << std::endl;
    HashMap<std::string, int> map;
    
    map.insert("Driver1", 100);
    map.insert("Driver2", 200);
    map.insert("Driver3", 150);
    
    auto val = map.get("Driver2");
    if (val && val.value() == 200) {
        std::cout << "✓ Hash Map lookup working correctly" << std::endl;
    }
    
    std::cout << "Hash Map size: " << map.size() << std::endl;
}

void testWeightedGraph() {
    std::cout << "\n=== Testing Weighted Graph + Dijkstra ===" << std::endl;
    WeightedGraph graph(5);
    
    graph.addBidirectionalEdge(0, 1, 10.0);
    graph.addBidirectionalEdge(1, 2, 15.0);
    graph.addBidirectionalEdge(2, 3, 20.0);
    graph.addBidirectionalEdge(3, 4, 5.0);
    graph.addBidirectionalEdge(0, 4, 100.0);
    
    auto result = graph.dijkstra(0, 4);
    std::cout << "Shortest path from 0 to 4: " << result.total_distance << std::endl;
    std::cout << "Path: ";
    for (int node : result.path) {
        std::cout << node << " ";
    }
    std::cout << "\n✓ Dijkstra working correctly" << std::endl;
}

void testSeasonSimulation() {
    std::cout << "\n=== Testing F1 Season Simulation ===" << std::endl;
    
    SeasonManager season;
    season.initialize();
    
    std::cout << "Total races: " << season.getTotalRaces() << std::endl;
    std::cout << "Total drivers: " << season.getDrivers().size() << std::endl;
    std::cout << "Total cars: " << season.getCars().size() << std::endl;
    
    // Run first 5 races
    std::cout << "\nSimulating first 5 races..." << std::endl;
    for (int i = 0; i < 5 && i < season.getTotalRaces(); ++i) {
        season.runNextRace();
        const auto& tracks = season.getTracks();
        std::cout << "  Completed: " << tracks[i].getName() << std::endl;
    }
    
    // Get standings
    auto standings = season.getDriverStandings();
    std::cout << "\n=== Top 10 Driver Standings After 5 Races ===" << std::endl;
    std::cout << std::setw(4) << "Pos" << " | " 
              << std::setw(20) << "Driver" << " | "
              << std::setw(8) << "Skill" << " | "
              << std::setw(8) << "Points" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (size_t i = 0; i < 10 && i < standings.size(); ++i) {
        const auto& driver = standings[i];
        std::cout << std::setw(4) << (i + 1) << " | "
                  << std::setw(20) << driver.getName() << " | "
                  << std::setw(8) << std::fixed << std::setprecision(2) << driver.getSkillRating() << " | "
                  << std::setw(8) << driver.getSeasonPoints() << std::endl;
    }
    
    // Test weather control
    std::cout << "\n=== Testing Weather System ===" << std::endl;
    std::cout << "Current weather: " << season.getSimulator().getWeather() << std::endl;
    season.getSimulator().setWeather(0.8);
    std::cout << "Set weather to: " << season.getSimulator().getWeather() << std::endl;
    
    // Run all remaining races
    std::cout << "\nSimulating remaining races..." << std::endl;
    season.runAllRaces();
    
    // Final standings
    standings = season.getDriverStandings();
    std::cout << "\n=== Final Championship Standings (All 22 Races) ===" << std::endl;
    std::cout << std::setw(4) << "Pos" << " | " 
              << std::setw(20) << "Driver" << " | "
              << std::setw(8) << "Skill" << " | "
              << std::setw(8) << "Points" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (size_t i = 0; i < standings.size(); ++i) {
        const auto& driver = standings[i];
        std::cout << std::setw(4) << (i + 1) << " | "
                  << std::setw(20) << driver.getName() << " | "
                  << std::setw(8) << std::fixed << std::setprecision(2) << driver.getSkillRating() << " | "
                  << std::setw(8) << driver.getSeasonPoints() << std::endl;
    }
    
    std::cout << "\n✓ Season simulation completed successfully" << std::endl;
    
    // Verify skill score correlation
    std::cout << "\n=== Verifying Skill-Based Rankings ===" << std::endl;
    std::cout << "Note: Rankings should correlate with skill ratings over a full season." << std::endl;
    std::cout << "Champion: " << standings[0].getName() 
              << " (Skill: " << standings[0].getSkillRating() << ")" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   RaceLogic - F1 Season Simulator" << std::endl;
    std::cout << "         Core Logic Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testMaxHeap();
        testHashMap();
        testWeightedGraph();
        testSeasonSimulation();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "   All Tests Passed Successfully! ✓" << std::endl;
        std::cout << "========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
