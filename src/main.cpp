#include "SeasonManager.h"
#include <iomanip>
#include <iostream>

int main() {
  std::cout << "Initializing RaceLogic v0.1 (Data-Driven Mode)..." << std::endl;

  SeasonManager season;

  // Register Drivers from file
  season.loadDriversFromFile("data/drivers.txt");

  // Load Race Events
  season.loadRaceEvents("data/race_events.txt");

  std::cout << "Drivers and Events Registered." << std::endl;

  // Simulate Season (5 Races)
  for (int race = 1; race <= 5; ++race) {
    std::cout << "\n===================================" << std::endl;
    std::cout << "STARTING RACE " << race << std::endl;
    std::cout << "===================================" << std::endl;

    // Simulate 60 Laps per race
    for (int lap = 1; lap <= 60; ++lap) {
      season.processRaceLap(race, lap);
    }

    // End Race (Prints results and assigns points)
    season.endRace();
  }

  // Final Season Standings
  std::cout << "\n\n###################################" << std::endl;
  std::cout << "FINAL SEASON STANDINGS" << std::endl;
  std::cout << "###################################" << std::endl;

  Vector<Driver *> standings = season.getSeasonStandings();
  for (int i = 0; i < standings.size(); ++i) {
    Driver *d = standings[i];
    std::cout << (i + 1) << ". " << d->getName() << " [" << d->getTeam()
              << "] - " << d->getSeasonPoints()
              << " PTS [Total Time: " << std::fixed << std::setprecision(2)
              << d->getSeasonTotalTime() << "s]" << std::endl;
  }

  // Export Results
  season.exportSeasonToJson("dashboard/data.js");

  return 0;
}
