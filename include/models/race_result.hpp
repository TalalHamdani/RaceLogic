#pragma once

#include <string>

class RaceResult {
public:
    RaceResult() 
        : driver_id_(0), position_(0), actual_time_(0.0), 
          skill_score_(0.0), points_(0) {}
    
    RaceResult(int driver_id, int position, double actual_time, 
               double skill_score, int points)
        : driver_id_(driver_id), position_(position), 
          actual_time_(actual_time), skill_score_(skill_score), 
          points_(points) {}

    int getDriverId() const { return driver_id_; }
    int getPosition() const { return position_; }
    double getActualTime() const { return actual_time_; }
    double getSkillScore() const { return skill_score_; }
    int getPoints() const { return points_; }

    void setDriverId(int id) { driver_id_ = id; }
    void setPosition(int pos) { position_ = pos; }
    void setActualTime(double time) { actual_time_ = time; }
    void setSkillScore(double score) { skill_score_ = score; }
    void setPoints(int pts) { points_ = pts; }

private:
    int driver_id_;
    int position_;
    double actual_time_;
    double skill_score_;
    int points_;
};
