import pandas as pd
import glob
import numpy as np

# ==========================================
# 1. KNOWLEDGE BASE (Mapping IDs to Names/Teams)
# ==========================================
# Includes standard 2024 grid + mid-season replacements (COL, LAW, BEA)
DRIVER_DB = {
    'VER': {'name': 'Max Verstappen', 'team': 'Red Bull', 'tier': 1},
    'PER': {'name': 'Sergio Perez',   'team': 'Red Bull', 'tier': 1},
    'HAM': {'name': 'Lewis Hamilton', 'team': 'Mercedes', 'tier': 1},
    'RUS': {'name': 'George Russell', 'team': 'Mercedes', 'tier': 1},
    'LEC': {'name': 'Charles Leclerc','team': 'Ferrari',  'tier': 1},
    'SAI': {'name': 'Carlos Sainz',   'team': 'Ferrari',  'tier': 1},
    'NOR': {'name': 'Lando Norris',   'team': 'McLaren',  'tier': 1},
    'PIA': {'name': 'Oscar Piastri',  'team': 'McLaren',  'tier': 1},
    'ALO': {'name': 'Fernando Alonso','team': 'Aston Martin', 'tier': 2},
    'STR': {'name': 'Lance Stroll',   'team': 'Aston Martin', 'tier': 2},
    'GAS': {'name': 'Pierre Gasly',   'team': 'Alpine',   'tier': 2},
    'OCO': {'name': 'Esteban Ocon',   'team': 'Alpine',   'tier': 2},
    'ALB': {'name': 'Alex Albon',     'team': 'Williams', 'tier': 3},
    'SAR': {'name': 'Logan Sargeant', 'team': 'Williams', 'tier': 3},
    'COL': {'name': 'Franco Colapinto','team': 'Williams','tier': 3}, # Replacement
    'TSU': {'name': 'Yuki Tsunoda',   'team': 'RB',       'tier': 2},
    'RIC': {'name': 'Daniel Ricciardo','team': 'RB',       'tier': 2},
    'LAW': {'name': 'Liam Lawson',    'team': 'RB',       'tier': 2}, # Replacement
    'BOT': {'name': 'Valtteri Bottas','team': 'Kick Sauber','tier': 3},
    'ZHO': {'name': 'Guanyu Zhou',    'team': 'Kick Sauber','tier': 3},
    'HUL': {'name': 'Nico Hulkenberg','team': 'Haas',     'tier': 3},
    'MAG': {'name': 'Kevin Magnussen','team': 'Haas',     'tier': 3},
    'BEA': {'name': 'Oliver Bearman', 'team': 'Haas',     'tier': 3}, # Reserve
    'DOO': {'name': 'Jack Doohan',    'team': 'Alpine',   'tier': 3}  # Reserve
}

# Values for the last column (Pit/Reflex) based on Tier
TIER_PIT_TIMES = {1: 18.5, 2: 19.8, 3: 21.0}

OUTPUT_FILE = 'drivers.txt'
SUFFIX_SUMMARY = '_driver_summary.csv'

def main():
    # 1. Find all summary files
    files = glob.glob(f"*{SUFFIX_SUMMARY}")
    if not files:
        print("No summary files found. Cannot calculate stats.")
        return

    print(f"Reading {len(files)} race summary files...")

    # 2. Aggregating Performance Data
    driver_stats = {} # Stores list of normalized scores per driver

    for filepath in files:
        try:
            df = pd.read_csv(filepath)
            
            # Filter out rows with missing lap times
            if 'AvgLapTime' not in df.columns:
                continue
            
            # Clean data (ensure numeric)
            df['AvgLapTime'] = pd.to_numeric(df['AvgLapTime'], errors='coerce')
            df = df.dropna(subset=['AvgLapTime'])
            
            if df.empty: 
                continue

            # Find the best average lap time (Benchmark for this race)
            best_time = df['AvgLapTime'].min()

            for _, row in df.iterrows():
                driver = row['Driver']
                avg_time = row['AvgLapTime']
                
                # Calculate Efficiency Ratio (1.0 = Perfect, Lower = Slower)
                # e.g., Winner 98s, Driver 100s -> Ratio 0.98
                ratio = best_time / avg_time
                
                if driver not in driver_stats:
                    driver_stats[driver] = []
                driver_stats[driver].append(ratio)
                
        except Exception as e:
            print(f"Error reading {filepath}: {e}")

    # 3. Compute Final Stats
    final_output = []
    
    # Calculate global averages
    driver_averages = {}
    for driver, ratios in driver_stats.items():
        driver_averages[driver] = np.mean(ratios)

    # Normalize to user's scale (0.78 to 0.95)
    # Find min and max performance in the dataset
    if not driver_averages:
        print("No driver data extracted.")
        return

    min_perf = min(driver_averages.values())
    max_perf = max(driver_averages.values())
    
    # Target scale
    TARGET_MIN = 0.78
    TARGET_MAX = 0.95

    # List of drivers to write (All found in CSVs)
    # We sort them by performance (Highest skill first)
    sorted_drivers = sorted(driver_averages.items(), key=lambda x: x[1], reverse=True)

    print("-" * 30)
    print("Detected Drivers & Calculated Skill:")
    
    for driver_id, raw_score in sorted_drivers:
        # Get Static Info
        info = DRIVER_DB.get(driver_id, {'name': f"Driver {driver_id}", 'team': 'Unknown', 'tier': 3})
        
        # 1. Calculate Skill (Map raw_score to 0.78-0.95 range)
        if max_perf == min_perf:
            skill = 0.85 # Default if only 1 driver exists
        else:
            skill = TARGET_MIN + ((raw_score - min_perf) * (TARGET_MAX - TARGET_MIN) / (max_perf - min_perf))
        
        # Round to 2 decimals
        skill_str = f"{skill:.2f}"

        # 2. Get Pit Value (Based on Tier + slight random variation or fixed)
        # Using the base values you provided in the example (Tier based)
        base_pit = TIER_PIT_TIMES.get(info['tier'], 20.0)
        
        # Format Line: ID, Name, Team, Skill, PitVal
        line = f"{driver_id},{info['name']},{info['team']},{skill_str},{base_pit}"
        final_output.append(line)
        print(f"  {driver_id}: {skill_str} (Tier {info['tier']})")

    # 4. Write to file
    with open(OUTPUT_FILE, 'w') as f:
        for line in final_output:
            f.write(line + '\n')

    print("-" * 30)
    print(f"Successfully generated {OUTPUT_FILE} with {len(final_output)} drivers.")

if __name__ == "__main__":
    main()