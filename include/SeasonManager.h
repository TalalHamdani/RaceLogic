#ifndef SEASON_MANAGER_H
#define SEASON_MANAGER_H

#include "DriverRegistry.h"
#include "HashMap.h"
#include "MaxHeap.h"

#include "ScoringEngine.h"

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
  std::string status; // "Finished", "DNF", "+1 Lap", etc.
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

  int currentRaceIndex;
  int totalRaces;
  float currentWeather;

  std::vector<std::string> trackNames; // Use std::vector

  // RaceID -> Lap -> DriverID -> List of Events
  // BSTMap<int, BSTMap<int, BSTMap<string, vector<Event>>>>
  // Optimized Data Structure:
  // RaceID (Vector Index) -> Lap (Vector Index) -> DriverID (HashMap) -> Events
  // Simpler and faster than triple-nested BST.
  std::vector<std::vector<HashMap<std::string, std::vector<Event>>>> raceEvents;

  // RaceID (Vector Index) -> DriverID (HashMap) -> List of Pit Laps
  std::vector<HashMap<std::string, std::vector<int>>>
      pitStops; // Use std::vector

  // RaceID -> Weather Value (0.0 - 1.0)
  std::vector<float> raceWeathers;

  std::vector<RaceResult> seasonHistory; // Use std::vector
  HashMap<std::string, float> idealTimes;

public:
  // Helper for Ideal Lap Time
  void loadIdealLapTimes(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open())
      return;
    std::string line;
    while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string track, timeStr;
      if (std::getline(ss, track, ',') && std::getline(ss, timeStr, ',')) {
        try {
          idealTimes.put(track, std::stof(timeStr));
        } catch (...) {
        }
      }
    }
  }

  float getIdealLapTime(const std::string &track, float weatherVal) {
    float baseTime = 90.0f; // Default fallback

    // Find closest match in the map (e.g., "Abu Dhabi" matches "Yas Marina")
    // For now, simpler exact match or substring check if we iterate keys.
    // Since our map structure doesn't support iteration easily without forEach,
    // and keys are simple strings, let's try direct lookup first, then
    // fallback.

    // We can't easily do substring search on keys in HashMap without exposing
    // internal iterator. However, the keys in file are "Abu Dhabi", "Brazil",
    // etc. The track passed in comes from `trackNames` vector which matches
    // file keys (hopefully).

    // Hardcoded check for known keys to map input string to key if necessary,
    // OR just rely on consistent naming.
    // Let's rely on consistent naming and simple "contains".

    // Check known keys based on substring
    std::string key = "";
    if (track.find("Abu Dhabi") != std::string::npos)
      key = "Abu Dhabi";
    else if (track.find("Brazil") != std::string::npos)
      key = "Brazil";
    else if (track.find("Las Vegas") != std::string::npos)
      key = "Las Vegas";
    else if (track.find("Mexico") != std::string::npos)
      key = "Mexico";
    else if (track.find("Qatar") != std::string::npos)
      key = "Qatar";

    if (key != "" && idealTimes.contains(key)) {
      baseTime = idealTimes[key];
    }

    if (weatherVal > 0.1f) {
      return baseTime * 1.15f; // +15% for wet
    }
    return baseTime;
  }

  // Get Total Laps for a specific race
  int getRaceLapCount(int raceId) {
    // 0-indexed internally, but user passes 1-indexed ID
    if (raceId < 1 || raceId > (int)trackNames.size())
      return 60;

    // Map ID 1..5 to the known laps for those specific 5 files.
    if (raceId == 1)
      return 58; // Abu Dhabi
    if (raceId == 2)
      return 71; // Brazil
    if (raceId == 3)
      return 50; // Las Vegas
    if (raceId == 4)
      return 71; // Mexico
    if (raceId == 5)
      return 57; // Qatar

    return 60;
  }

  SeasonManager() {
    registry = new DriverRegistry();
    leaderboard = new MaxHeap();

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
  }

  ~SeasonManager() {
    delete registry;
    delete leaderboard;
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
    // Clear existing pit stop data
    pitStops.clear();

    // Resize pitStops vector to match raceEvents size if necessary
    if (raceEvents.size() > pitStops.size()) {
      pitStops.resize(raceEvents.size());
    }

    // Iterate through raceEvents to find PIT events
    for (size_t raceId = 0; raceId < raceEvents.size(); ++raceId) {
      for (size_t lapNum = 0; lapNum < raceEvents[raceId].size(); ++lapNum) {
        // Access the HashMap for the current raceId and lapNum
        HashMap<std::string, std::vector<Event>> &driversMap =
            raceEvents[raceId][lapNum];

        // Iterate through drivers in the HashMap
        driversMap.forEach(
            [&](const std::string &driverId, std::vector<Event> &events) {
              for (const auto &ev : events) {
                if (ev.type == "PIT") {
                  // Ensure pitStops[raceId] is valid before accessing
                  if (raceId < pitStops.size()) {
                    pitStops[raceId][driverId].push_back(lapNum);
                  }
                }
              }
            });
      }
    }

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

          // Check for TRACK,NAME event in batch mode
          // Format: BATCH,raceId,lap,TRACK:NAME:TrackName
          if (parts.size() >= 4 && parts[3].rfind("TRACK:NAME:", 0) == 0) {
            std::string trackNameValue =
                parts[3].substr(11); // "TRACK:NAME:".length()
            if (raceId >= (int)trackNames.size()) {
              trackNames.resize(raceId + 1); // Resize to accommodate raceId
            }
            trackNames[raceId] = trackNameValue;
            continue; // Processed this line, move to next
          }

          // Dynamically resize raceEvents vector
          if (raceId >= (int)raceEvents.size()) {
            raceEvents.resize(raceId + 1);
          }
          if (lap >= (int)raceEvents[raceId].size()) {
            raceEvents[raceId].resize(lap + 1);
          }

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

            if (type == "COMPOUND" || (driverId == "TRACK" && type == "NAME")) {
              detail = valStr; // Treat as string
            } else {
              val = std::stof(valStr);
            }

            if (driverId == "TRACK" && type == "NAME") {
              if (raceId > (int)trackNames.size()) {
                trackNames.resize(raceId);
              }
              trackNames[raceId - 1] = detail; // Use string value from parsing
            } else if (type == "WEATHER") {
              if (raceId >= (int)raceWeathers.size()) {
                raceWeathers.resize(raceId + 1);
              }
              raceWeathers[raceId] = val;
            } else {
              // Ensure Vector sizing
              if (raceId >= (int)raceEvents.size()) {
                raceEvents.resize(raceId + 1);
              }
              if (lap >= (int)raceEvents[raceId].size()) {
                raceEvents[raceId].resize(lap + 1);
              }

              if (type == "PIT") {
                if (raceId >= (int)pitStops.size()) {
                  pitStops.resize(raceId + 1);
                }
                pitStops[raceId][driverId].push_back(lap);
              }

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

  void startRace(int raceId) {
    if (raceId < (int)raceEvents.size() && raceEvents[raceId].size() > 0) {
      auto &gridEvents = raceEvents[raceId][0]; // Lap 0
      gridEvents.forEach(
          [&](const std::string &driverId, std::vector<Event> &events) {
            Driver *d = registry->getDriver(driverId);
            if (d) {
              d->markParticipated();
              // std::cout << "DEBUG: Initialized " << driverId << " from Grid."
              // << std::endl;
            }
          });
    }
  }

  void processRaceLap(int raceId, int lap) {
    // Safety check for Vector bounds
    if (raceId >= (int)raceEvents.size())
      return;
    if (lap >= (int)raceEvents[raceId].size())
      return;

    if (raceId < (int)raceWeathers.size()) {
      currentWeather = raceWeathers[raceId];
    } else {
      currentWeather = 0.0f;
    }

    // Access via reference
    auto &lapEvents = raceEvents[raceId][lap];

    // Registry forEach
    registry->forEach([&](Driver *d) {
      bool processedLap = false;
      // Check if driver has events in the HashMap
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
            d->recordOvertake();
          } else if (ev.type == "COMPOUND") {
            d->setTyreCompound(ev.detail);
          }
          d->markParticipated(); // Any event = Participation
        }
      }

      if (lapEvents.contains(d->getId())) {
        d->resetMissedLaps(); // Driver is active
      } else {
        // Missing Data Scenario
        // ONLY apply penalties if they have ALREADY participated in this race
        if (d->didParticipate()) {
          d->incrementMissedLaps();
          d->updateLapTime(120.0f); // Reduced penalty (Lapped pace)

          // Check if truly DNF (Missed > 3 consecutive laps)
          if (d->getConsecutiveMissedLaps() > 3) {
            d->setRankingScore(0.0f); // Confirm DNF
          }
        }
        // If they haven't participated yet, do nothing (they aren't in this
        // race)
      }

      // Calculate Degradation
      if (processedLap) {
        int nextPitLap = getRaceLapCount(raceId); // Dynamic end of race

        // Check if pitStops has info for this race/driver
        if (raceId < (int)pitStops.size() &&
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
      Driver *d = leaderboard->pop();
      // Only include drivers who actually participated (had events)
      if (d->didParticipate()) {
        raceResults.push_back(d);
      } else {
        // If they didn't participate, just reset state (safety)
        d->resetRaceState();
      }
    }

    // New Safety Reset for ALL drivers in registry
    // This ensures that even drivers NOT in the leaderboard (dropped) are
    // reset.
    registry->forEach([&](Driver *d) {
      if (!d->didParticipate()) {
        d->resetRaceState();
        d->forceUnparticipate();
      }
    });

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
    if (currentResult.raceId < (int)raceWeathers.size()) {
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

      std::string status =
          (d->getConsecutiveMissedLaps() > 3) ? "DNF" : "Finished";

      currentResult.results.push_back(
          {d->getName(), d->getTeam(), pts, d->getRaceTotalTime(),
           d->getPitStops(), d->getOvertakes(), (int)d->getTyreDegradation(),
           d->getRankingScore(), (int)(i + 1), status});

      d->resetRaceState();
      d->forceUnparticipate();
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
             << "\"pits\": " << res.pitStops << ", "
             << "\"status\": \"" << res.status << "\"}";
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
