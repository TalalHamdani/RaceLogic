import random

drivers = [
    "VER", "HAM", "PER", "RUS", "SAI", "ALB", "ALO", "PIA", "LEC", "NOR",
    "GAS", "STR", "ZHO", "RIC", "BOT", "OCO", "TSU", "MAG", "SAR", "HUL"
]

def generate_race_data(filename):
    with open(filename, 'w') as f:
        for race in range(1, 6): # 5 Races
            num_laps = random.randint(57, 60)
            
            # Initial Positions
            random.shuffle(drivers)
            for i, driver in enumerate(drivers):
                f.write(f"{race},1,{driver},POS,{i+1}\n")

            # Laps
            for lap in range(1, num_laps + 1):
                batch_line = [f"BATCH,{race},{lap}"]
                
                for driver in drivers:
                    # Random lap time around 90s +/- variance
                    base_time = 90.0
                    variance = random.uniform(-2.0, 3.0)
                    lap_time = round(base_time + variance, 3)
                    batch_line.append(f"{driver}:{lap_time}")
                
                f.write(",".join(batch_line) + "\n")
                
                # Random Events (Pit Stops / Overtakes)
                if random.random() < 0.1: # 10% chance of event per lap
                    event_driver = random.choice(drivers)
                    event_type = random.choice(["PIT", "OVERTAKE"])
                    val = 20.0 if event_type == "PIT" else 1
                    f.write(f"{race},{lap},{event_driver},{event_type},{val}\n")

    print(f"Generated data for 5 races in {filename}")

if __name__ == "__main__":
    generate_race_data("data/race_events.txt")
