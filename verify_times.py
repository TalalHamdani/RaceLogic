
import pandas as pd

def check_time(file_path, driver_id):
    try:
        df = pd.read_csv(file_path)
        # Convert to numeric, coerce errors
        df['LapTimeSeconds'] = pd.to_numeric(df['LapTimeSeconds'], errors='coerce')
        
        # Filter for driver
        driver_df = df[df['Driver'] == driver_id]
        
        # Sum
        total_time = driver_df['LapTimeSeconds'].sum()
        count = driver_df['LapTimeSeconds'].count()
        
        print(f"Driver: {driver_id}")
        print(f"Laps Counted: {count}")
        print(f"Total Time (Sum): {total_time:.4f}")
        
    except Exception as e:
        print(f"Error: {e}")

check_time('race_data_csv/Abu Dhabi_lap_times.csv', 'NOR')
