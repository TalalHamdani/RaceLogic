#pragma once

#include <string>

class Driver {
public:
    Driver() : id_(0), name_(""), skill_rating_(1.0), season_points_(0) {}
    
    Driver(int id, const std::string& name, double skill_rating)
        : id_(id), name_(name), skill_rating_(skill_rating), season_points_(0) {}

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    double getSkillRating() const { return skill_rating_; }
    int getSeasonPoints() const { return season_points_; }

    void setId(int id) { id_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setSkillRating(double rating) { skill_rating_ = rating; }
    void addSeasonPoints(int points) { season_points_ += points; }
    void setSeasonPoints(int points) { season_points_ = points; }

private:
    int id_;
    std::string name_;
    double skill_rating_;  // Base skill (0.8 - 1.2, where 1.0 is average)
    int season_points_;
};
