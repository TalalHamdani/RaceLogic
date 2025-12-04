#pragma once

#include <memory>
#include "imgui.h"
#include "core/season_manager.hpp"

class RaceLogicUI {
public:
    RaceLogicUI() : season_manager_(std::make_unique<SeasonManager>()) {
        season_manager_->initialize();
    }

    void render() {
        renderMainWindow();
        renderDriverStandings();
        renderRaceResults();
        renderSimulationControls();
    }

    SeasonManager* getSeasonManager() { return season_manager_.get(); }

private:
    std::unique_ptr<SeasonManager> season_manager_;
    bool show_driver_standings_ = true;
    bool show_race_results_ = true;
    bool show_controls_ = true;

    void renderMainWindow() {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("RaceLogic - F1 Season Simulator");
        
        ImGui::Text("F1 Season Simulator");
        ImGui::Separator();
        
        int current_race = season_manager_->getCurrentRace();
        int total_races = season_manager_->getTotalRaces();
        ImGui::Text("Race Progress: %d / %d", current_race, total_races);
        
        float progress = total_races > 0 ? static_cast<float>(current_race) / total_races : 0.0f;
        ImGui::ProgressBar(progress, ImVec2(-1, 0));
        
        ImGui::Separator();
        
        double weather = season_manager_->getSimulator().getWeather();
        ImGui::Text("Weather: %.2f (0.0 = Dry, 1.0 = Heavy Rain)", weather);
        
        ImGui::Checkbox("Driver Standings", &show_driver_standings_);
        ImGui::SameLine();
        ImGui::Checkbox("Race Results", &show_race_results_);
        ImGui::SameLine();
        ImGui::Checkbox("Controls", &show_controls_);
        
        ImGui::End();
    }

    void renderDriverStandings() {
        if (!show_driver_standings_) return;

        ImGui::SetNextWindowPos(ImVec2(420, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Driver Championship Standings", &show_driver_standings_);
        
        auto standings = season_manager_->getDriverStandings();
        
        ImGui::Text("Rankings by Season Points (Max-Heap Sorted)");
        ImGui::Separator();
        
        if (ImGui::BeginTable("StandingsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Pos", ImGuiTableColumnFlags_WidthFixed, 40.0f);
            ImGui::TableSetupColumn("Driver", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Skill", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Points", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableHeadersRow();

            for (size_t i = 0; i < standings.size(); ++i) {
                const auto& driver = standings[i];
                ImGui::TableNextRow();
                
                ImGui::TableNextColumn();
                ImGui::Text("%zu", i + 1);
                
                ImGui::TableNextColumn();
                ImGui::Text("%s", driver.getName().c_str());
                
                ImGui::TableNextColumn();
                ImGui::Text("%.2f", driver.getSkillRating());
                
                ImGui::TableNextColumn();
                ImGui::Text("%d", driver.getSeasonPoints());
            }
            
            ImGui::EndTable();
        }
        
        ImGui::End();
    }

    void renderRaceResults() {
        if (!show_race_results_) return;

        ImGui::SetNextWindowPos(ImVec2(930, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 600), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Latest Race Results", &show_race_results_);
        
        const auto& all_results = season_manager_->getRaceResults();
        
        if (all_results.empty()) {
            ImGui::Text("No races completed yet. Click 'Run Next Race' to start.");
        } else {
            int race_idx = static_cast<int>(all_results.size()) - 1;
            const auto& results = all_results[race_idx];
            const auto& tracks = season_manager_->getTracks();
            
            if (race_idx < static_cast<int>(tracks.size())) {
                ImGui::Text("Race %d: %s", race_idx + 1, tracks[race_idx].getName().c_str());
            }
            ImGui::Separator();
            
            if (ImGui::BeginTable("ResultsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Pos", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableSetupColumn("Driver", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Skill Score", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Points", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                ImGui::TableHeadersRow();

                const auto& drivers = season_manager_->getDrivers();
                
                for (const auto& result : results) {
                    ImGui::TableNextRow();
                    
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", result.getPosition());
                    
                    ImGui::TableNextColumn();
                    // Find driver name
                    for (const auto& driver : drivers) {
                        if (driver.getId() == result.getDriverId()) {
                            ImGui::Text("%s", driver.getName().c_str());
                            break;
                        }
                    }
                    
                    ImGui::TableNextColumn();
                    int minutes = static_cast<int>(result.getActualTime()) / 60;
                    int seconds = static_cast<int>(result.getActualTime()) % 60;
                    int millis = static_cast<int>((result.getActualTime() - static_cast<int>(result.getActualTime())) * 1000);
                    ImGui::Text("%d:%02d.%03d", minutes, seconds, millis);
                    
                    ImGui::TableNextColumn();
                    ImGui::Text("%.4f", result.getSkillScore());
                    
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", result.getPoints());
                }
                
                ImGui::EndTable();
            }
        }
        
        ImGui::End();
    }

    void renderSimulationControls() {
        if (!show_controls_) return;

        ImGui::SetNextWindowPos(ImVec2(10, 170), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Simulation Controls", &show_controls_);
        
        if (ImGui::Button("Run Next Race", ImVec2(180, 30))) {
            season_manager_->runNextRace();
        }
        ImGui::SameLine();
        if (ImGui::Button("Run All Races", ImVec2(180, 30))) {
            season_manager_->runAllRaces();
        }
        
        if (ImGui::Button("Reset Season", ImVec2(180, 30))) {
            season_manager_->resetSeason();
        }
        
        ImGui::Separator();
        ImGui::Text("Weather Control (Continuous)");
        
        float weather = static_cast<float>(season_manager_->getSimulator().getWeather());
        if (ImGui::SliderFloat("Weather", &weather, 0.0f, 1.0f, "%.2f")) {
            season_manager_->getSimulator().setWeather(static_cast<double>(weather));
        }
        ImGui::Text("0.0 = Dry Conditions");
        ImGui::Text("1.0 = Heavy Rain");
        
        ImGui::End();
    }
};
