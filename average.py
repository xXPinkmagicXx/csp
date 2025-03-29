import pandas as pd
import argparse
import glob
import os

def average_csv_values(input_folders, output_file):
    csv_files = {}
    
    for folder in input_folders:
        for file in glob.glob(os.path.join(folder, "*.csv")):
            filename = os.path.basename(file)
            if filename not in csv_files:
                csv_files[filename] = []
            csv_files[filename].append(file)
    
    if not csv_files:
        print("No CSV files found in the given directories.")
        return
    
    for filename, files in csv_files.items():
        dataframes = [pd.read_csv(file) for file in files]
    
        column_names = dataframes[0].columns.tolist()
        print(f"Processing {filename} with columns: {column_names}")
        
        if len(column_names) < 2:
            print(f"Skipping {filename} as it does not have enough columns.")
            continue
        
        key_col, value_col = column_names[:2]
        merged_df = dataframes[0][[key_col]].copy()
        
        values_list = [df[value_col] for df in dataframes]
        merged_df[value_col] = pd.concat(values_list, axis=1).mean(axis=1).round(5)
        
        output_path = os.path.join(output_file, filename)
        merged_df.to_csv(output_path, index=False)
        print(f"Averaged CSV file saved as {output_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Average values from matching CSV files in multiple folders.")
    parser.add_argument("output_folder", type=str, help="Folder to save the output CSV files")
    parser.add_argument("input_folders", nargs='+', help="List of input folders containing CSV files")
    args = parser.parse_args()
    
    os.makedirs(args.output_folder, exist_ok=True)
    average_csv_values(args.input_folders, args.output_folder)