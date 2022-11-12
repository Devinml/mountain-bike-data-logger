import os
import pandas as pd
import matplotlib.pyplot as plt

def read_file(file_name):
    return pd.read_csv(f'data/{file_name}')

def make_plot(df, plot_bool):
    fig, ax = plt.subplots()
    df.plot(x='millis', ax=ax)
    if plot:
        plt.show()

def calculate_time_ran(df):
    first = df['millis'].iloc[0]
    last = df['millis'].iloc[-1]
    print(f'{(last-first) / 1000} seconds')

def compute_differences(df):
    df['difference'] = df['millis'].diff()
    return df

def calculate_logging_statistics(df):
    df = compute_differences(df)
    print('log statisics')
    print(f'mean: {df["difference"].mean()}')
    print(f'variance: {df["difference"].var()}')
    print(f'max: {df["difference"].max()}')
    print(f'min: {df["difference"].min()}')
    print(f'total time: {calculate_time_ran(df)}')
    try:
        print(f"log_interval: {df['LOG_INTERVAL'].iloc[0]}")
        print(f'sync interval {df["SYNC_INTERVAL"].iloc[0]}')
        print(f'sync_time {df["syncTime"].iloc[0]}')
    except Exception as e:
        print("intervals not found")
    print('\n')

def plot_indexs_for_dir():
    for file_ in os.listdir('data'):
        df = read_file(file_)
        df = compute_differences(df)

        # plot_index_interval(df, file_, 'difference')

def compute_stats_of_all_files():
    for file_ in os.listdir('data'):
        print(f'file: {file_}')
        df = read_file(file_)
        calculate_logging_statistics(df)

def plot_index_interval(df, file_name, column='millis'):
    fig, ax = plt.subplots()
    df[column].plot(ax=ax)
    split_name = file_name.split('.')[0]
    plt.savefig(f'imgs/{split_name}.jpeg')

if __name__ == '__main__':
    # compute_stats_of_all_files()
    # plot_indexs_for_dir()
    file_name = '154017.csv'
    plot = True
    df = read_file(file_name)
    calculate_logging_statistics(df)
    make_plot(df, plot_bool=plot)
    plot_cols = ['x_1', "y_1", "z_1", "x_2", "y_2", "z_2"]
    plot_index_interval(df, file_name, plot_cols)
    # calculate_time_ran(df)
