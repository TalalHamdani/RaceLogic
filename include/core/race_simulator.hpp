#pragma once

#include <vector>
#include <random>
#include "models/driver.hpp"
#include "models/car.hpp"
#include "models/track.hpp"
#include "models/race_result.hpp"
#include "core/max_heap.hpp"
#include "core/hash_map.hpp"

class RaceSimulator {
public:
    struct DriverCarPair {
        Driver* driver;
        Car* car;
        
        DriverCarPair() : driver(nullptr), car(nullptr) {}
        DriverCarPair(Driver* d, Car* c) : driver(d), car(c) {}
    };

    RaceSimulator() : rng_(std::random_device{}()), weather_(0.5) {}

    void setWeather(double weather) {
        weather_ = std::max(0.0, std::min(1.0, weather));  // Clamp to [0, 1]
    }

    double getWeather() const {
        return weather_;
    }

    // Simulate a single race
    std::vector<RaceResult> simulateRace(
        const std::vector<DriverCarPair>& entries,
        const Track& track
    ) {
        std::vector<RaceResult> results;
        results.reserve(entries.size());

        // Calculate actual time and skill score for each driver
        for (size_t i = 0; i < entries.size(); ++i) {
            const auto& entry = entries[i];
            if (!entry.driver || !entry.car) continue;

            double actual_time = calculateActualTime(
                entry.driver, 
                entry.car, 
                track
            );

            double skill_score = calculateSkillScore(
                entry.driver,
                entry.car,
                track,
                actual_time
            );

            results.push_back(RaceResult(
                entry.driver->getId(),
                0,  // Position will be set after sorting
                actual_time,
                skill_score,
                0   // Points will be assigned based on position
            ));
        }

        // Sort by skill score (higher is better)
        std::sort(results.begin(), results.end(), 
            [](const RaceResult& a, const RaceResult& b) {
                return a.getSkillScore() > b.getSkillScore();
            });

        // Assign positions and points
        const std::vector<int> points_table = {25, 18, 15, 12, 10, 8, 6, 4, 2, 1};
        for (size_t i = 0; i < results.size(); ++i) {
            results[i].setPosition(static_cast<int>(i + 1));
            if (i < points_table.size()) {
                results[i].setPoints(points_table[i]);
            }
        }

        return results;
    }

private:
    std::mt19937 rng_;
    double weather_;  // 0.0 = dry, 1.0 = heavy rain

    // Calculate actual lap time with all factors
    double calculateActualTime(
        const Driver* driver,
        const Car* car,
        const Track& track
    ) {
        double base_time = track.getIdealLapTime() * track.getNumLaps();
        
        // Random variation factor (±5%)
        std::uniform_real_distribution<> dist(0.95, 1.05);
        double random_factor = dist(rng_);

        // Driver skill (inverse - better drivers are faster)
        double driver_factor = 2.0 - driver->getSkillRating();  // 0.8 skill -> 1.2x time, 1.2 skill -> 0.8x time
        
        // Car performance (inverse - better cars are faster)
        double car_factor = 2.0 - car->getPerformanceRating();

        // Weather impact (0.0 = no impact, 1.0 = +30% time)
        double weather_factor = 1.0 + (weather_ * 0.3);

        // Tyre degradation (increases over laps)
        std::uniform_real_distribution<> tyre_dist(1.0, 1.15);
        double tyre_factor = tyre_dist(rng_);

        // Fuel load (starts heavy, gets lighter - affects early laps more)
        std::uniform_real_distribution<> fuel_dist(1.0, 1.08);
        double fuel_factor = fuel_dist(rng_);

        // Traffic/overtaking factor
        std::uniform_real_distribution<> traffic_dist(1.0, 1.05);
        double traffic_factor = traffic_dist(rng_);

        // Combined time calculation
        double actual_time = base_time * random_factor * driver_factor * 
                           car_factor * weather_factor * tyre_factor * 
                           fuel_factor * traffic_factor;

        return actual_time;
    }

    // Calculate skill score based on ideal vs actual time and factors
    double calculateSkillScore(
        const Driver* driver,
        const Car* car,
        const Track& track,
        double actual_time
    ) {
        double ideal_time = track.getIdealLapTime() * track.getNumLaps();
        
        // Base score: ideal/actual ratio
        double base_score = ideal_time / actual_time;

        // Factor 1: Driver skill rating
        double skill_factor = driver->getSkillRating();

        // Factor 2: Car performance (normalize to 0-1 range bonus)
        double car_factor = (car->getPerformanceRating() - 0.85) / 0.3;  // 0.85-1.15 -> 0-1
        
        // Factor 3: Weather adaptation (drivers perform differently in rain)
        std::uniform_real_distribution<> weather_adapt_dist(0.85, 1.15);
        double weather_adaptation = weather_adapt_dist(rng_);
        double weather_factor = 1.0 - (weather_ * 0.3 * (2.0 - weather_adaptation));

        // Factor 4: Tyre management skill
        std::uniform_real_distribution<> tyre_skill_dist(0.9, 1.1);
        double tyre_management = tyre_skill_dist(rng_);

        // Factor 5: Fuel management
        std::uniform_real_distribution<> fuel_skill_dist(0.95, 1.05);
        double fuel_management = fuel_skill_dist(rng_);

        // Factor 6: Overtaking skill
        std::uniform_real_distribution<> overtake_dist(0.9, 1.1);
        double overtaking_skill = overtake_dist(rng_);

        // Factor 7: Consistency (race-to-race)
        std::uniform_real_distribution<> consistency_dist(0.95, 1.05);
        double consistency = consistency_dist(rng_);

        // Combined skill score (weighted)
        double final_score = base_score * 
                           (skill_factor * 0.3) *        // 30% driver skill
                           (1.0 + car_factor * 0.2) *    // 20% car performance
                           weather_factor *              // Weather adaptation
                           tyre_management *             // Tyre management
                           fuel_management *             // Fuel management
                           overtaking_skill *            // Overtaking
                           consistency;                  // Consistency

        return final_score;
    }
};
