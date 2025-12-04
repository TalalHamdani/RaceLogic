#pragma once

#include <string>
#include <memory>
#include "core/weighted_graph.hpp"

class Track {
public:
    Track() : id_(0), name_(""), ideal_lap_time_(0.0), num_laps_(0) {}
    
    Track(int id, const std::string& name, double ideal_lap_time, int num_laps)
        : id_(id), name_(name), ideal_lap_time_(ideal_lap_time), num_laps_(num_laps) {
        buildTrackGraph();
    }

    // Copy constructor
    Track(const Track& other) 
        : id_(other.id_), name_(other.name_), 
          ideal_lap_time_(other.ideal_lap_time_), num_laps_(other.num_laps_) {
        buildTrackGraph();
    }

    // Move constructor
    Track(Track&& other) noexcept
        : id_(other.id_), name_(std::move(other.name_)), 
          ideal_lap_time_(other.ideal_lap_time_), num_laps_(other.num_laps_),
          track_graph_(std::move(other.track_graph_)) {}

    // Copy assignment
    Track& operator=(const Track& other) {
        if (this != &other) {
            id_ = other.id_;
            name_ = other.name_;
            ideal_lap_time_ = other.ideal_lap_time_;
            num_laps_ = other.num_laps_;
            buildTrackGraph();
        }
        return *this;
    }

    // Move assignment
    Track& operator=(Track&& other) noexcept {
        if (this != &other) {
            id_ = other.id_;
            name_ = std::move(other.name_);
            ideal_lap_time_ = other.ideal_lap_time_;
            num_laps_ = other.num_laps_;
            track_graph_ = std::move(other.track_graph_);
        }
        return *this;
    }

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    double getIdealLapTime() const { return ideal_lap_time_; }
    int getNumLaps() const { return num_laps_; }
    const WeightedGraph* getTrackGraph() const { return track_graph_.get(); }

    void setId(int id) { id_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setIdealLapTime(double time) { ideal_lap_time_ = time; }
    void setNumLaps(int laps) { num_laps_ = laps; }

private:
    int id_;
    std::string name_;
    double ideal_lap_time_;  // Ideal lap time in seconds
    int num_laps_;
    std::unique_ptr<WeightedGraph> track_graph_;

    void buildTrackGraph() {
        // Build a simple graph representing track sectors
        // For simplicity, we create 10 sectors per lap
        int sectors_per_lap = 10;
        track_graph_ = std::make_unique<WeightedGraph>(sectors_per_lap);
        
        // Create a circular path through sectors
        double sector_time = ideal_lap_time_ / sectors_per_lap;
        for (int i = 0; i < sectors_per_lap - 1; ++i) {
            track_graph_->addEdge(i, i + 1, sector_time);
        }
        // Close the loop
        track_graph_->addEdge(sectors_per_lap - 1, 0, sector_time);
    }
};
