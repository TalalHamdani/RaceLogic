import random

def generate_events():
    drivers = [
        ("VER", "Red Bull", 0.95), ("HAM", "Mercedes", 0.90), ("NOR", "McLaren", 0.88),
        ("LEC", "Ferrari", 0.89), ("PIA", "McLaren", 0.87), ("RUS", "Mercedes", 0.88),
        ("SAI", "Ferrari", 0.88), ("ALB", "Williams", 0.82), ("SAR", "Williams", 0.78),
        ("PER", "Red Bull", 0.88), ("ALO", "Aston Martin", 0.86), ("STR", "Aston Martin", 0.80),
        ("GAS", "Alpine", 0.81), ("OCO", "Alpine", 0.81), ("TSU", "RB", 0.80),
        ("RIC", "RB", 0.80), ("BOT", "Kick Sauber", 0.79), ("ZHO", "Kick Sauber", 0.78),
        ("HUL", "Haas", 0.79), ("MAG", "Haas", 0.79)
    ]
    
    # 5 Races data: (RaceID, BaseLapTime)
    races = [
        (1, 92.0),  # Bahrain
        (2, 88.0),  # Saudi
        (3, 80.0),  # Australia
        (4, 90.0),  # Japan
        (5, 95.0)   # China
    ]
    
    events = []
    
    for race_id, base_time in races:
        # Initial Positions (Random qualifying shuffle based on speed + noise)
        qualifying = []
        for d in drivers:
            score = d[2] * 100 + random.uniform(-5, 5)
            qualifying.append((d[0], score))
        
        qualifying.sort(key=lambda x: x[1], reverse=True)
        current_positions = [d[0] for d in qualifying]
        
        # Log Initial Positions (Lap 0 equivalent or start of Lap 1)
        for rank, (d_id, _) in enumerate(qualifying):
            events.append(f"{race_id},1,{d_id},POS,{rank + 1}")

        driver_states = {d[0]: {"tyre_age": 0} for d in drivers}

        for lap in range(1, 21): # 20 Laps
            lap_times = []
            
            for d_id in current_positions:
                # Find driver stats
                driver_stats = next(d for d in drivers if d[0] == d_id)
                speed_factor = driver_stats[2] # 0.78 - 0.95
                
                # Lap Time Simulation
                # Slower drivers (low speed factor) take MORE time.
                # Time = Base / SpeedFactor
                # e.g. 90 / 0.9 = 100s. 90 / 0.8 = 112s.
                lap_time = base_time / (0.8 + (speed_factor * 0.2)) 
                lap_time += random.uniform(-0.5, 0.5) # Variance
                
                # Tyre Wear
                tyre_age = driver_states[d_id]["tyre_age"]
                wear_penalty = (tyre_age / 20.0) ** 2 * 2.0 # Up to 2s loss
                lap_time += wear_penalty
                
                # Pit Stop Logic
                event_type = "LAP"
                event_val = lap_time
                
                # Pit around lap 8-12 for everyone
                if 8 <= lap <= 12 and tyre_age > 8:
                    if random.random() < 0.3: # 30% chance per eligible lap
                        pit_duration = 20.0 + random.uniform(-1, 1)
                        event_type = "PIT"
                        event_val = pit_duration
                        driver_states[d_id]["tyre_age"] = -1 # Reset (will define next lap start at 0)
                        
                driver_states[d_id]["tyre_age"] += 1
                
                events.append(f"{race_id},{lap},{d_id},{event_type},{event_val:.3f}")
                lap_times.append((d_id, lap_time))
            
            # Simulate Overtakes (Simple shuffle for demo)
            if lap > 1 and random.random() < 0.5:
                # Pick a random spot to swap
                idx = random.randint(0, len(current_positions) - 2)
                p1 = current_positions[idx]
                p2 = current_positions[idx+1]
                
                # Swap
                current_positions[idx], current_positions[idx+1] = current_positions[idx+1], current_positions[idx]
                
                events.append(f"{race_id},{lap},{p2},OVERTAKE,{idx+1}") # p2 overtook p1 (p2 moved up)
                events.append(f"{race_id},{lap},{p2},POS,{idx+1}")
                events.append(f"{race_id},{lap},{p1},POS,{idx+2}")

    with open("data/race_events.txt", "w") as f:
        f.write("\n".join(events))

if __name__ == "__main__":
    generate_events()
    print("race_events.txt generated.")
