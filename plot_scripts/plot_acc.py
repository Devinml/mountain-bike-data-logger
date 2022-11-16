import os
import pandas as pd
from matplotlib.figure import figaspect
import matplotlib.pyplot as plt

# plt.style.use("ggplot")

def read_file(file_name):
    return pd.read_csv(f'{file_name}')

def make_plot(df, plot_bool=False):
    fig, ax = plt.subplots()
    df.plot(x='time', ax=ax)
    if plot_bool:
        plt.show()

def calculate_time_ran(df):
    first = df['time'].iloc[0]
    last = df['time'].iloc[-1]
    print(f'{(last-first) / 1000} seconds')

def compute_differences(df):
    df['difference'] = df['time'].diff()
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

def plot_acc(df, file_name, col='x1', save=False):
    fig, ax = plt.subplots()
    df.plot(x='time', y=col, ax=ax)
    plt.title(f"ACC {col} file_name: {file_name}")
    plt.tight_layout()

def plot_indexs_for_dir():
    for file_ in os.listdir('data/parsed_bin_files/'):
        full_file_path = 'data/parsed_bin_files/' + file_
        df = read_file(full_file_path)
        df = compute_differences    (df)
        plot_index_interval(df, file_, 'difference')
        for col in ["x1", "y1", "z1", "x2", "y2", "z2"]:
            plot_acc(df, file_name=file_, col=col)


def compute_stats_of_all_files():
    for file_ in os.listdir('data/parsed_bin_files/'):
        print(f'file: {file_}')
        full_file_path = 'data/parsed_bin_files/' + file_
        df = read_file(full_file_path)
        calculate_logging_statistics(df)

def plot_index_interval(df, file_name, column='time'):
    fig, ax = plt.subplots()
    df[column].plot(ax=ax)
    split_name = file_name.split('.')[0]
    plt.savefig(f'imgs/{split_name}.jpeg')

def plot_z1_vs_z2(df):
    
    w, h = figaspect(16/9)
    fig, ax = plt.subplots(figsize=(w,h))
    ax.set_ylabel("(m/s^2)")
    ax.set_xlabel("time (millis)")
    ax.plot(df['time'], df['z1'], label='lower')
    ax.set_xlim(33749, 34255)
    ax.plot(df['time'], df['z2'], label='upper')
    plt.tight_layout()

    ax.legend()

if __name__ == '__main__':
    # compute_stats_of_all_files()
    # plot_indexs_for_dir()
    df = read_file('data/parsed_bin_files/110554_hard.csv')
    plot_z1_vs_z2(df)
    
    plt.show()
    # file_name = '154017.csv'
    # plot = True
    # df = read_file(file_name)
    # calculate_logging_statistics(df)
    # make_plot(df, plot_bool=plot)
    # plot_cols = ['x_1', "y_1", "z_1", "x_2", "y_2", "z_2"]
    # plot_index_interval(df, file_name, plot_cols)
    # calculate_time_ran(df)
