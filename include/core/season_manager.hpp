#pragma once

#include <vector>
#include <memory>
#include <string>
#include "models/driver.hpp"
#include "models/car.hpp"
#include "models/track.hpp"
#include "models/race_result.hpp"
#include "core/race_simulator.hpp"
#include "core/hash_map.hpp"
#include "core/max_heap.hpp"

class SeasonManager {
public:
    SeasonManager() : current_race_(0), total_races_(0) {}

    void initialize() {
        initializeDrivers();
        initializeCars();
        initializeTracks();
        assignDriversToCars();
    }

    void runNextRace() {
        if (current_race_ >= total_races_) {
            return;
        }

        const Track& track = tracks_[current_race_];
        
        // Prepare driver-car pairs
        std::vector<RaceSimulator::DriverCarPair> entries;
        for (auto& driver : drivers_) {
            auto car_opt = driver_car_map_.get(driver.getId());
            if (car_opt.has_value()) {
                int car_id = car_opt.value();
                entries.push_back(RaceSimulator::DriverCarPair(&driver, &cars_[car_id]));
            }
        }

        // Simulate race
        auto results = simulator_.simulateRace(entries, track);
        
        // Update season points
        for (const auto& result : results) {
            for (auto& driver : drivers_) {
                if (driver.getId() == result.getDriverId()) {
                    driver.addSeasonPoints(result.getPoints());
                    break;
                }
            }
        }

        race_results_.push_back(results);
        current_race_++;
    }

    void runAllRaces() {
        while (current_race_ < total_races_) {
            runNextRace();
        }
    }

    void resetSeason() {
        current_race_ = 0;
        race_results_.clear();
        for (auto& driver : drivers_) {
            driver.setSeasonPoints(0);
        }
    }

    const std::vector<Driver>& getDrivers() const { return drivers_; }
    const std::vector<Car>& getCars() const { return cars_; }
    const std::vector<Track>& getTracks() const { return tracks_; }
    int getCurrentRace() const { return current_race_; }
    int getTotalRaces() const { return total_races_; }
    RaceSimulator& getSimulator() { return simulator_; }
    
    const std::vector<std::vector<RaceResult>>& getRaceResults() const {
        return race_results_;
    }

    std::vector<Driver> getDriverStandings() const {
        std::vector<Driver> standings = drivers_;
        std::sort(standings.begin(), standings.end(),
            [](const Driver& a, const Driver& b) {
                return a.getSeasonPoints() > b.getSeasonPoints();
            });
        return standings;
    }

    // Get live rankings using max heap
    std::vector<Driver> getLiveRankings() const {
        MaxHeap<Driver> heap([](const Driver& a, const Driver& b) {
            return a.getSeasonPoints() > b.getSeasonPoints();
        });

        for (const auto& driver : drivers_) {
            heap.insert(driver);
        }

        std::vector<Driver> rankings;
        while (!heap.isEmpty()) {
            rankings.push_back(heap.extractMax());
        }

        return rankings;
    }

private:
    std::vector<Driver> drivers_;
    std::vector<Car> cars_;
    std::vector<Track> tracks_;
    HashMap<int, int> driver_car_map_;  // driver_id -> car_id
    RaceSimulator simulator_;
    std::vector<std::vector<RaceResult>> race_results_;
    int current_race_;
    int total_races_;

    void initializeDrivers() {
        // Create 20 F1 drivers with varying skill levels
        drivers_ = {
            Driver(0, "Max Verstappen", 1.15),
            Driver(1, "Lewis Hamilton", 1.12),
            Driver(2, "Charles Leclerc", 1.08),
            Driver(3, "Sergio Perez", 1.05),
            Driver(4, "Carlos Sainz", 1.06),
            Driver(5, "George Russell", 1.07),
            Driver(6, "Lando Norris", 1.04),
            Driver(7, "Fernando Alonso", 1.10),
            Driver(8, "Oscar Piastri", 1.02),
            Driver(9, "Lance Stroll", 0.98),
            Driver(10, "Pierre Gasly", 1.00),
            Driver(11, "Esteban Ocon", 0.99),
            Driver(12, "Alexander Albon", 1.01),
            Driver(13, "Yuki Tsunoda", 0.97),
            Driver(14, "Valtteri Bottas", 1.03),
            Driver(15, "Zhou Guanyu", 0.93),
            Driver(16, "Kevin Magnussen", 0.96),
            Driver(17, "Nico Hulkenberg", 0.98),
            Driver(18, "Logan Sargeant", 0.90),
            Driver(19, "Daniel Ricciardo", 1.01),
        };
    }

    void initializeCars() {
        // Create 10 teams (2 cars each)
        cars_ = {
            Car(0, "Red Bull Racing", 1.15),
            Car(1, "Red Bull Racing", 1.15),
            Car(2, "Mercedes", 1.10),
            Car(3, "Mercedes", 1.10),
            Car(4, "Ferrari", 1.08),
            Car(5, "Ferrari", 1.08),
            Car(6, "McLaren", 1.05),
            Car(7, "McLaren", 1.05),
            Car(8, "Aston Martin", 1.03),
            Car(9, "Aston Martin", 1.03),
            Car(10, "Alpine", 0.98),
            Car(11, "Alpine", 0.98),
            Car(12, "Williams", 0.92),
            Car(13, "Williams", 0.92),
            Car(14, "Alfa Romeo", 0.95),
            Car(15, "Alfa Romeo", 0.95),
            Car(16, "Haas", 0.93),
            Car(17, "Haas", 0.93),
            Car(18, "AlphaTauri", 0.96),
            Car(19, "AlphaTauri", 0.96),
        };
    }

    void initializeTracks() {
        // Create 22 race tracks with varying lap times
        tracks_ = {
            Track(0, "Bahrain", 91.0, 57),
            Track(1, "Saudi Arabia", 90.7, 50),
            Track(2, "Australia", 80.2, 58),
            Track(3, "Azerbaijan", 103.0, 51),
            Track(4, "Miami", 90.0, 57),
            Track(5, "Monaco", 72.9, 78),
            Track(6, "Spain", 78.2, 66),
            Track(7, "Canada", 73.1, 70),
            Track(8, "Austria", 65.0, 71),
            Track(9, "Britain", 87.1, 52),
            Track(10, "Hungary", 77.0, 70),
            Track(11, "Belgium", 107.0, 44),
            Track(12, "Netherlands", 72.3, 72),
            Track(13, "Italy", 81.0, 53),
            Track(14, "Singapore", 102.0, 61),
            Track(15, "Japan", 90.0, 53),
            Track(16, "Qatar", 84.0, 57),
            Track(17, "USA", 96.0, 56),
            Track(18, "Mexico", 78.0, 71),
            Track(19, "Brazil", 70.5, 71),
            Track(20, "Las Vegas", 96.0, 50),
            Track(21, "Abu Dhabi", 87.0, 58),
        };
        total_races_ = static_cast<int>(tracks_.size());
    }

    void assignDriversToCars() {
        // Assign each driver to a car (1:1 mapping)
        for (size_t i = 0; i < drivers_.size() && i < cars_.size(); ++i) {
            driver_car_map_.insert(drivers_[i].getId(), cars_[i].getId());
        }
    }
};
