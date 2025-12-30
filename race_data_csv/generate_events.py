import pandas as pd
import glob
import os

# ==========================================
# CONFIGURATION
# ==========================================
OUTPUT_FILE = 'race_events.txt'

# Filename suffixes
SUFFIX_LAPS = '_lap_times.csv'
SUFFIX_STINTS = '_stints.csv'
SUFFIX_SUMMARY = '_driver_summary.csv'

def get_races():
    """Finds all unique race names and assigns them a RaceID."""
    files = glob.glob(f"race_data_csv/*{SUFFIX_LAPS}")
    races = []
    for f in files:
        # Extract "Las Vegas" from "race_data_csv\\Las Vegas_lap_times.csv" or forward slash
        basename = os.path.basename(f)
        name = basename.replace(SUFFIX_LAPS, '')
        races.append(name)
    
    # Sort alphabetically to keep IDs consistent
    races.sort()
    
    # Return a dictionary: {'Abu Dhabi': 1, 'Brazil': 2, ...}
    return {name: i+1 for i, name in enumerate(races)}

def load_data(race_name):
    """Loads the 3 CSVs for a specific race."""
    files = {
        'laps': f"race_data_csv/{race_name}{SUFFIX_LAPS}",
        'stints': f"race_data_csv/{race_name}{SUFFIX_STINTS}",
        'summary': f"race_data_csv/{race_name}{SUFFIX_SUMMARY}"
    }
    dfs = {}
    for key, path in files.items():
        if os.path.exists(path):
            dfs[key] = pd.read_csv(path)
        else:
            dfs[key] = pd.DataFrame()
    return dfs

def generate_events(race_id, race_name, dfs):
    """Generates event lines for a specific RaceID."""
    events = []

    # --- 0. Track Name (Lap 0) ---
    # This fixes the dashboard showing wrong track names
    name_line = f"{race_id},0,TRACK,NAME,{race_name}"
    events.append({'lap': 0, 'sort': 0, 'line': name_line})

    # --- 1. Weather (Lap 0) ---
    if not dfs['summary'].empty:
        w_text = str(dfs['summary'].iloc[0].get('Weather', ''))
        val = 1.0 if 'Rain' in w_text and 'Not' not in w_text else 0.0
        line = f"{race_id},0,TRACK,WEATHER,{val}"
        events.append({'lap': 0, 'sort': 0, 'line': line})

    # --- 2. Tires (Start of Stint) ---
    if not dfs['stints'].empty:
        stints = dfs['stints']
        required = {'Driver', 'Compound', 'StartLap'}
        if required.issubset(stints.columns):
            for _, row in stints.iterrows():
                lap = int(row['StartLap'])
                target_lap = 0 if lap == 1 else lap
                driver = row['Driver']
                compound = str(row['Compound']).capitalize()
                print(f"DEBUG: Race {race_id} Stint Driver={driver} Lap={lap}")
                line = f"{race_id},{target_lap},{driver},COMPOUND,{compound}"
                events.append({'lap': target_lap, 'sort': 1, 'line': line})
                
                # Generate PIT event for subsequent stints (Pit stop happens on previous lap)
                if lap > 1:
                    pit_lap = lap - 1
                    # Assume ~20s pit time for now, or use data if available
                    pit_line = f"{race_id},{pit_lap},{driver},PIT,0.0"
                    events.append({'lap': pit_lap, 'sort': 1, 'line': pit_line})

    # --- 3. Laps: Positions and Batches ---
    if not dfs['laps'].empty:
        laps_df = dfs['laps'].sort_values(by='LapNumber')
        
        # Clean Data
        laps_df['LapTimeSeconds'] = pd.to_numeric(laps_df['LapTimeSeconds'], errors='coerce')
        laps_df = laps_df.dropna(subset=['LapTimeSeconds'])

        driver_totals = {}
        prev_positions = {} # Driver -> Rank
        grouped = laps_df.groupby('LapNumber')
        
        for lap_num, group in grouped:
            lap = int(lap_num)
            lap_data = []
            
            for _, row in group.iterrows():
                driver = row['Driver']
                time_sec = row['LapTimeSeconds']
                
                # Update total time safely
                driver_totals[driver] = driver_totals.get(driver, 0.0) + time_sec
                
                lap_data.append({
                    'driver': driver,
                    'total': driver_totals[driver],
                    'lap_time': time_sec
                })
            
            # Sort by total time (lowest is P1)
            lap_data.sort(key=lambda x: x['total'])
            
            # A) POS rows
            for rank, d in enumerate(lap_data, start=1):
                # Format total time to 3 decimal places to avoid messy floats
                line = f"{race_id},{lap},{d['driver']},POS,{rank}"
                events.append({'lap': lap, 'sort': 2, 'line': line})
                
                # Check for Overtakes
                if lap > 1:
                    prev_rank = prev_positions.get(d['driver'])
                    if prev_rank and rank < prev_rank:
                        # Gained position (e.g. P3 -> P2)
                        overtake_line = f"{race_id},{lap},{d['driver']},OVERTAKE,1"
                        events.append({'lap': lap, 'sort': 2, 'line': overtake_line})
            
            # Update previous positions for next lap
            current_positions = {d['driver']: i+1 for i, d in enumerate(lap_data)}
            prev_positions = current_positions
            
            # B) BATCH row
            batch_items = [f"{d['driver']}:{d['lap_time']}" for d in lap_data]
            batch_str = ",".join(batch_items)
            
            batch_line = f"BATCH,{race_id},{lap},{batch_str}"
            events.append({'lap': lap, 'sort': 3, 'line': batch_line})

    events.sort(key=lambda x: (x['lap'], x['sort']))
    return [e['line'] for e in events]

# ==========================================
# MAIN EXECUTION
# ==========================================
def main():
    race_map = get_races()
    
    if not race_map:
        print("No '_lap_times.csv' files found. Check your folder.")
        return

    print(f"Found {len(race_map)} races. Assigning IDs:")
    for name, rid in race_map.items():
        print(f"  ID {rid}: {name}")

    all_lines = []

    # Loop through races sorted by ID
    for name, rid in sorted(race_map.items(), key=lambda item: item[1]):
        print(f"Processing RaceID {rid} ({name})...")
        
        dfs = load_data(name)
        lines = generate_events(rid, name, dfs)
        all_lines.extend(lines)

    # Write final file
    with open(OUTPUT_FILE, 'w') as f:
        for line in all_lines:
            f.write(line + '\n')

    print("-" * 30)
    print(f"Success! {len(all_lines)} lines written to {OUTPUT_FILE}")

if __name__ == "__main__":
    main()