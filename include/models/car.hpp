#pragma once

#include <string>

class Car {
public:
    Car() : id_(0), team_name_(""), performance_rating_(1.0) {}
    
    Car(int id, const std::string& team_name, double performance_rating)
        : id_(id), team_name_(team_name), performance_rating_(performance_rating) {}

    int getId() const { return id_; }
    const std::string& getTeamName() const { return team_name_; }
    double getPerformanceRating() const { return performance_rating_; }

    void setId(int id) { id_ = id; }
    void setTeamName(const std::string& name) { team_name_ = name; }
    void setPerformanceRating(double rating) { performance_rating_ = rating; }

private:
    int id_;
    std::string team_name_;
    double performance_rating_;  // Car performance multiplier (0.85 - 1.15)
};
