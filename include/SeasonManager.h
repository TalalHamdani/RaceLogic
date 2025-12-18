#ifndef SEASON_MANAGER_H
#define SEASON_MANAGER_H

#include "BSTMap.h"
#include "DriverRegistry.h"
#include "MaxHeap.h"
#include "RaceGraph.h"
#include "ScoringEngine.h"
#include "Vector.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


struct Event {
  std::string type;   // LAP, PIT, POS, OVERTAKE
  float value;        // Time or Position
  std::string detail; // Extra info (e.g., Tyre Compound Name)
};

struct DriverResult {
  std::string name;
  std::string team;
  int points;
  float totalTime;
  int pitStops;
  int overtakes;
  int tyreDegradation;
  float score;
  int position;
};

struct RaceResult {
  int raceId;
  std::string trackName;
  std::string weather;
  std::vector<DriverResult> results; // Use std::vector
};

class SeasonManager {
private:
  DriverRegistry *registry;
  MaxHeap *leaderboard;
  RaceGraph *currentTrack;

  int currentRaceIndex;
  int totalRaces;
  float currentWeather;

  std::vector<std::string> trackNames; // Use std::vector

  // RaceID -> Lap -> DriverID -> List of Events
  // BSTMap<int, BSTMap<int, BSTMap<string, vector<Event>>>>
  BSTMap<int, BSTMap<int, BSTMap<std::string, std::vector<Event>>>>
      raceEvents; // Use std::vector

  // RaceID -> DriverID -> List of Pit Laps
  BSTMap<int, BSTMap<std::string, std::vector<int>>>
      pitStops; // Use std::vector

  // RaceID -> Weather Value (0.0 - 1.0)
  BSTMap<int, float> raceWeathers;

  std::vector<RaceResult> seasonHistory; // Use std::vector

public:
  SeasonManager() {
    registry = new DriverRegistry();
    leaderboard = new MaxHeap();
    currentTrack = new RaceGraph(10);
    currentRaceIndex = 0;
    totalRaces = 20;
    currentWeather = 0.0f;

    trackNames.push_back("Bahrain");
    trackNames.push_back("Saudi Arabia");
    trackNames.push_back("Australia");
    trackNames.push_back("Japan");
    trackNames.push_back("China");
    trackNames.push_back("Miami");
    trackNames.push_back("Imola");
    trackNames.push_back("Monaco");
    trackNames.push_back("Canada");
    trackNames.push_back("Spain");

    for (int i = 0; i < 10; i++) {
      currentTrack->addEdge(i, (i + 1) % 10, 10.0f, 0.5f);
    }
  }

  ~SeasonManager() {
    delete registry;
    delete leaderboard;
    delete currentTrack;
  }

  void loadDriversFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Failed to open file: " << filename << std::endl;
      return;
    }

    std::string line;
    while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string id, name, team, speedStr, pitTimeStr;

      if (std::getline(ss, id, ',') && std::getline(ss, name, ',') &&
          std::getline(ss, team, ',') && std::getline(ss, speedStr, ',')) {

        float pitTime = 20.0f;
        if (std::getline(ss, pitTimeStr, ',')) {
          try {
            pitTime = std::stof(pitTimeStr);
          } catch (...) {
          }
        }

        try {
          float speed = std::stof(speedStr);
          registerDriver(id, name, team, speed, pitTime);
        } catch (...) {
          std::cerr << "Error parsing speed for driver: " << name << std::endl;
        }
      }
    }
    file.close();
    std::cout << "Drivers loaded from " << filename << std::endl;
  }

  void analyzePitStops() {
    pitStops.clear();
    // We need to iterate over everything. BSTMap supports forEach
    raceEvents.forEach(
        [&](int raceId,
            BSTMap<int, BSTMap<std::string, std::vector<Event>>> &lapsMap) {
          lapsMap.forEach(
              [&](int lapNum,
                  BSTMap<std::string, std::vector<Event>> &driversMap) {
                driversMap.forEach(
                    [&](std::string driverId, std::vector<Event> &events) {
                      for (auto &ev : events) {
                        if (ev.type == "PIT") {
                          pitStops[raceId][driverId].push_back(lapNum);
                        }
                      }
                    });
              });
        });

    std::cout << "Pit strategies analyzed." << std::endl;
  }

  void loadRaceEvents(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Failed to open event file: " << filename << std::endl;
      return;
    }

    std::string line;
    while (std::getline(file, line)) {
      std::stringstream ss(line);

      // BATCH MODE
      if (line.rfind("BATCH,", 0) == 0) {
        std::string segment;
        std::vector<std::string> parts;
        while (std::getline(ss, segment, ',')) {
          parts.push_back(segment);
        }

        if (parts.size() < 4)
          continue;

        try {
          int raceId = std::stoi(parts[1]);
          int lap = std::stoi(parts[2]);

          for (size_t i = 3; i < parts.size(); ++i) {
            std::string item = parts[i];
            size_t colonPos = item.find(':');
            if (colonPos != std::string::npos) {
              std::string driverId = item.substr(0, colonPos);
              float time = std::stof(item.substr(colonPos + 1));
              raceEvents[raceId][lap][driverId].push_back({"LAP", time, ""});
            }
          }
        } catch (...) {
        }

      } else {
        // STANDARD MODE
        std::string raceIdStr, lapStr, driverId, type, valStr;
        if (std::getline(ss, raceIdStr, ',') && std::getline(ss, lapStr, ',') &&
            std::getline(ss, driverId, ',') && std::getline(ss, type, ',') &&
            std::getline(ss, valStr, ',')) {

          try {
            int raceId = std::stoi(raceIdStr);
            int lap = std::stoi(lapStr);

            float val = 0.0f;
            std::string detail = "";

            if (type == "COMPOUND") {
              detail = valStr; // Direct string assignment
            } else {
              val = std::stof(valStr);
            }

            if (type == "WEATHER") {
              raceWeathers[raceId] = val;
            } else {
              raceEvents[raceId][lap][driverId].push_back({type, val, detail});
            }
          } catch (...) {
          }
        }
      }
    }
    file.close();
    std::cout << "Race Events loaded from " << filename << std::endl;
    analyzePitStops();
  }

  void registerDriver(std::string id, std::string name, std::string team,
                      float speed, float pitTime) {
    Driver *d = new Driver(id, name, team, speed, pitTime);
    registry->addDriver(d);
    leaderboard->push(d);
  }

  void processRaceLap(int raceId, int lap) {
    // Check if we have events for this lap using contains()
    if (!raceEvents.contains(raceId) || !raceEvents[raceId].contains(lap)) {
      return;
    }

    if (raceWeathers.contains(raceId)) {
      currentWeather = raceWeathers[raceId];
    } else {
      currentWeather = 0.0f;
    }

    // Access via reference to avoid copy
    auto &lapEvents = raceEvents[raceId][lap];

    // Registry forEach still works
    registry->forEach([&](Driver *d) {
      bool processedLap = false;

      // Check if driver has events
      if (lapEvents.contains(d->getId())) {
        std::vector<Event> &events = lapEvents[d->getId()];
        for (const auto &ev : events) {
          if (ev.type == "LAP") {
            d->updateLapTime(ev.value);
            processedLap = true;
          } else if (ev.type == "PIT") {
            d->addPitTime(ev.value);
            d->resetStint(lap);
          } else if (ev.type == "POS") {
            d->setRankingScore(1000.0f - ev.value);
          } else if (ev.type == "OVERTAKE") {
            d->incrementOvertakes();
          } else if (ev.type == "COMPOUND") {
            d->setTyreCompound(ev.detail);
          }
        }
      } else {
        // Simulate default lap
        d->updateLapTime(90.0f);
      }

      // Calculate Degradation
      if (processedLap) {
        int nextPitLap = 60; // Default end of race

        // Check if pitStops has info for this race/driver
        if (pitStops.contains(raceId) &&
            pitStops[raceId].contains(d->getId())) {
          std::vector<int> &stops = pitStops[raceId][d->getId()];
          for (const int stopLap : stops) {
            if (stopLap > lap) {
              nextPitLap = stopLap;
              break;
            }
          }
        }

        int lapsInStint = nextPitLap - d->getStintStartLap();
        if (lapsInStint <= 0)
          lapsInStint = 1;

        int currentStintLaps = lap - d->getStintStartLap();

        float progress = (float)currentStintLaps / (float)lapsInStint;
        float deg = progress * 80.0f;
        if (deg > 100.0f)
          deg = 100.0f;
        d->setTyreDegradation(deg);
      }
    });

    leaderboard->rebuild();
  }

  void endRace() {
    std::vector<Driver *> raceResults;
    while (!leaderboard->isEmpty()) {
      raceResults.push_back(leaderboard->pop());
    }

    int points[] = {25, 18, 15, 12, 10, 8, 6, 4, 2, 1};

    std::string trackName = (currentRaceIndex < (int)trackNames.size())
                                ? trackNames[currentRaceIndex]
                                : "Unknown Track";
    std::cout << "\n--- Race " << (currentRaceIndex + 1) << " Results ["
              << trackName << "] ---" << std::endl;

    RaceResult currentResult;
    currentResult.raceId = currentRaceIndex + 1;
    currentResult.trackName = trackName;

    // Determine Weather from Data
    float weatherVal = 0.0f;
    if (raceWeathers.contains(currentResult.raceId)) {
      weatherVal = raceWeathers[currentResult.raceId];
    }

    currentResult.weather = (weatherVal > 0.1f) ? "Rainy" : "Dry";
    std::cout << "Weather: " << currentResult.weather << std::endl;

    for (size_t i = 0; i < raceResults.size(); ++i) {
      Driver *d = raceResults[i];
      int pts = (i < 10) ? points[i] : 0;

      d->addSeasonPoints(pts);
      d->addSeasonTime(d->getRaceTotalTime());

      std::cout << (i + 1) << ". " << d->getName() << " (" << d->getTeam()
                << ") - " << pts << " pts [Score: " << d->getRankingScore()
                << "] [Time: " << d->getRaceTotalTime()
                << "s] [Pits: " << d->getPitStops()
                << "] [Tyres: " << d->getTyreCompound() << "]" << std::endl;

      currentResult.results.push_back(
          {d->getName(), d->getTeam(), pts, d->getRaceTotalTime(),
           d->getPitStops(), d->getOvertakes(), (int)d->getTyreDegradation(),
           d->getRankingScore(), (int)(i + 1)});

      d->resetRaceState();
    }

    seasonHistory.push_back(currentResult);

    for (Driver *d : raceResults) {
      leaderboard->push(d);
    }

    currentRaceIndex++;
  }

  std::vector<Driver *> getSeasonStandings() {
    std::vector<Driver *> standings;
    registry->forEach([&](Driver *d) { standings.push_back(d); });

    // We need to sort functionality. Vector doesn't have sort method built-in,
    // nor does it expose iterators compatible with std::sort if implemented
    // poorly. However, our Vector has begin()/end() pointers so std::sort
    // SHOULD work.
    std::sort(standings.begin(), standings.end(), [](Driver *a, Driver *b) {
      return a->getSeasonPoints() > b->getSeasonPoints();
    });

    return standings;
  }

  void exportSeasonToJson(const std::string &filename) {
    std::ofstream file(filename);
    file << "window.raceData = {\n";

    // 1. Export History (Custom Vector Iteration)
    file << "  \"races\": [\n";
    for (size_t i = 0; i < seasonHistory.size(); ++i) {
      const auto &race = seasonHistory[i];
      file << "    {\n";
      file << "      \"raceId\": " << race.raceId << ",\n";
      file << "      \"trackName\": \"" << race.trackName << "\",\n";
      file << "      \"weather\": \"" << race.weather << "\",\n";
      file << "      \"results\": [\n";

      for (size_t j = 0; j < race.results.size(); ++j) {
        const auto &res = race.results[j];
        file << "        {"
             << "\"position\": " << res.position << ", "
             << "\"name\": \"" << res.name << "\", "
             << "\"team\": \"" << res.team << "\", "
             << "\"points\": " << res.points << ", "
             << "\"time\": "
             << (std::isnan(res.totalTime) ? 0.0f : res.totalTime) << ", "
             << "\"overtakes\": " << res.overtakes << ", "
             << "\"tyreDegradation\": " << res.tyreDegradation << ", "
             << "\"pits\": " << res.pitStops << "}";
        if (j < race.results.size() - 1)
          file << ",";
        file << "\n";
      }
      file << "      ]\n";
      file << "    }";
      if (i < seasonHistory.size() - 1)
        file << ",";
      file << "\n";
    }
    file << "  ],\n";

    // 2. Export Standings
    file << "  \"standings\": [\n";
    auto standings = getSeasonStandings();
    for (size_t k = 0; k < standings.size(); ++k) {
      Driver *d = standings[k];
      file << "    {"
           << "\"rank\": " << (k + 1) << ", "
           << "\"name\": \"" << d->getName() << "\", "
           << "\"team\": \"" << d->getTeam() << "\", "
           << "\"points\": " << d->getSeasonPoints() << ", "
           << "\"totalTime\": "
           << (std::isnan(d->getSeasonTotalTime()) ? 0.0f
                                                   : d->getSeasonTotalTime())
           << "}";
      if (k < standings.size() - 1)
        file << ",";
      file << "\n";
    }
    file << "  ]\n";

    file << "};\n";
    file.close();
    std::cout << "Data exported to " << filename << std::endl;
  }

  // Getters
  MaxHeap *getLeaderboard() { return leaderboard; }
  DriverRegistry *getRegistry() { return registry; }
};

#endif // SEASON_MANAGER_H
