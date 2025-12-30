#ifndef DRIVER_H
#define DRIVER_H

#include <iostream>
#include <string>

class Driver {
private:
  std::string id;
  std::string name;
  std::string team;

  // Attributes
  float baseSpeed;
  float consistency;
  float wetWeatherSkill;
  float basePitTime;

  // Dynamic Race State
  float currentLapTime;
  float lastLapTime;
  float raceTotalTime;
  int tyreAge;
  float tyreDegradation;
  int stintStartLap;
  int pitStops;
  int overtakes;
  std::string tyreCompound;
  float rankingScore;

  // Track activity to distinguish DNF vs Lapped
  int consecutiveMissedLaps;
  bool hasParticipated; // Track if driver has any events in current race

  // Season State
  int seasonPoints;
  float seasonTotalTime;

public:
  Driver(std::string _id = "", std::string _name = "", std::string _team = "",
         float _speed = 0.5f, float _pitTime = 20.0f)
      : id(_id), name(_name), team(_team), baseSpeed(_speed), consistency(0.8f),
        wetWeatherSkill(0.5f), basePitTime(_pitTime), currentLapTime(0.0f),
        lastLapTime(0.0f), raceTotalTime(0.0f), tyreAge(0),
        tyreDegradation(0.0f), stintStartLap(0), pitStops(0), overtakes(0),
        tyreCompound("Soft"), rankingScore(0.0f), consecutiveMissedLaps(0),
        hasParticipated(false), // Initialize hasParticipated
        seasonPoints(0), seasonTotalTime(0.0f) {}

  // Getters
  std::string getId() const { return id; }
  std::string getName() const { return name; }
  std::string getTeam() const { return team; }
  float getRankingScore() const { return rankingScore; }
  float getRaceTotalTime() const { return raceTotalTime; }
  int getSeasonPoints() const { return seasonPoints; }
  float getSeasonTotalTime() const { return seasonTotalTime; }
  float getCurrentLapTime() const { return currentLapTime; }
  float getLastLapTime() const { return lastLapTime; }
  int getPitStops() const { return pitStops; }
  int getOvertakes() const { return overtakes; }
  float getTyreDegradation() const { return tyreDegradation; }
  int getTyreAge() const { return tyreAge; }
  int getStintStartLap() const { return stintStartLap; }
  float getWetWeatherSkill() const { return wetWeatherSkill; }
  std::string getTyreCompound() const { return tyreCompound; }
  float getConsistency() const { return consistency; }
  int getConsecutiveMissedLaps() const { return consecutiveMissedLaps; }
  bool didParticipate() const { return hasParticipated; }
  void markParticipated() { hasParticipated = true; }
  void forceUnparticipate() { hasParticipated = false; } // Force reset

  // Setters / Mutators
  void setRankingScore(float score) { rankingScore = score; }
  void updateLapTime(float time) {
    lastLapTime = currentLapTime;
    currentLapTime = time;
    raceTotalTime += time;
    tyreAge++;
    hasParticipated = true;
  }
  void addPitTime(float time) {
    currentLapTime += time;
    raceTotalTime += time;
    pitStops++;
    tyreAge = 0;
    tyreDegradation = 0.0f;
    hasParticipated = true;
  }
  void resetStint(int lap) { stintStartLap = lap; }
  void recordOvertake() {
    overtakes++;
    hasParticipated = true;
  }
  void setTyreDegradation(float deg) { tyreDegradation = deg; }
  void setTyreCompound(std::string comp) { tyreCompound = comp; }
  void addSeasonPoints(int pts) { seasonPoints += pts; }
  void addSeasonTime(float time) { seasonTotalTime += time; }

  void incrementMissedLaps() { consecutiveMissedLaps++; }
  void resetMissedLaps() { consecutiveMissedLaps = 0; }

  void resetRaceState() {
    currentLapTime = 0.0f;
    lastLapTime = 0.0f;
    raceTotalTime = 0.0f;
    tyreAge = 0;
    tyreDegradation = 0.0f;
    stintStartLap = 0;
    pitStops = 0;
    overtakes = 0;
    rankingScore = 0.0f;
    tyreCompound = "Soft";
    consecutiveMissedLaps = 0;
  }
};

#endif // DRIVER_H
