#ifndef SCORING_ENGINE_H
#define SCORING_ENGINE_H

#include "Driver.h"
#include <algorithm>
#include <cmath>

class ScoringEngine {
public:
  static float calculateScore(Driver *driver, float idealLapTime, float weather,
                              float trackDifficulty) {
    // 1. Speed Efficiency
    float actualTime = driver->getCurrentLapTime() > 0
                           ? driver->getCurrentLapTime()
                           : idealLapTime * 1.1f;
    float speedEfficiency = idealLapTime / actualTime;

    // 2. Tyre Degradation (Non-linear)
    float tyreDegFactor =
        1.0f + (std::pow(driver->getTyreAge() / 30.0f, 2) * 0.5f);
    float tyreAdjustedEfficiency = speedEfficiency / tyreDegFactor;

    // 3. Car Handicap (REMOVED)
    float carHandicap = 1.0f;

    // 4. Weather Skill
    float weatherBonus = 1.0f;
    if (weather > 0.5f) {
      weatherBonus = 1.0f + (driver->getWetWeatherSkill() * 0.2f);
    }

    // 5. Overtake Logic
    float overtakePotential = 10.0f * trackDifficulty;
    if (driver->getTyreCompound() == "Soft")
      overtakePotential *= 0.8f;
    if (driver->getTyreCompound() == "Hard")
      overtakePotential *= 1.3f;
    if (driver->getTyreAge() > 20)
      overtakePotential += 0.5f;

    float aggressionScore = overtakePotential * 0.1f;

    // 6. Consistency Bonus
    float consistencyBonus = 0.0f;
    if (std::abs(driver->getCurrentLapTime() - driver->getLastLapTime()) <
        0.1f) {
      consistencyBonus = 3.0f;
    }

    // Final Calculation
    float baseScore = tyreAdjustedEfficiency * 100.0f;
    float finalScore = (baseScore * carHandicap * weatherBonus) +
                       aggressionScore + consistencyBonus;

    return finalScore;
  }

  // Helper to simulate a lap time based on driver stats and track
  static float simulateLapTime(Driver *driver, float idealLapTime,
                               float weather) {
    // Base time + Random variance based on consistency
    float variance =
        (1.0f - driver->getConsistency()) * ((rand() % 100) / 100.0f);

    // Weather impact
    float weatherImpact =
        weather * (1.0f - driver->getWetWeatherSkill()) * 5.0f;

    // Tyre Deg impact on TIME
    float tyreDeg = 1.0f + (std::pow(driver->getTyreAge() / 30.0f, 2) * 0.1f);

    return idealLapTime * tyreDeg + weatherImpact + variance;
  }
};

#endif // SCORING_ENGINE_H
