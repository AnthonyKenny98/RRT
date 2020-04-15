"""Run python analysis using Intel Vtune software."""

# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-02 09:44:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-04-06 12:13:38

import csv
import json
from matplotlib import pyplot as plt
import numpy as np
import os
from pathlib import Path
import shutil
import subprocess
import sys

from plot import Figure


# Path in project of this file
DIR_PATH = os.path.dirname(os.path.realpath(__file__))

# Path for all test folders
TEST_PATH = DIR_PATH + '/tests'

# Path for test batch - updated in choose_test_batch()
batch_path = ''

# Required files and folders for each test batch
REQUIRED_FILES = ['tests.csv']
REQUIRED_FOLDERS = ['results', 'logs', 'reports', 'graphs']

# Parameters for params.h
PARAMS = ['XDIM', 'YDIM', 'ZDIM',
          'EPSILON', 'NUM_CONFIGS', 'RESOLUTION', 'GOAL_BIAS']

# Functions that we want to measure
ALLFUNCTIONS = [
    # 'benchmark',
    'rrt',
    'getRandomConfig',
    'findNearestConfig',
    'stepFromTo',
    'configCollisions',
    'edgeCollisions'
]
FUNCTIONS = [
    # 'benchmark',
    'getRandomConfig',
    'findNearestConfig',
    'stepFromTo',
    'configCollisions',
    'edgeCollisions'
]

COLORS = ['#999999', '#4285F4', '#34A853', '#FBBC05', '#EA4335']
# COLORS = '../../aux/colors/'


def get_colors():
    """Get colors from main project."""
    colors = []
    for i in range(1, 5):
        with open(COLORS + str(i) + '.txt') as f:
            colors.append(f.read())
    return colors


def call(command):
    """Wrap subprocess call for bash use."""
    subprocess.call(command, shell=True, executable='/bin/bash')


def choose_test_batch(argv):
    """Choose with test batch under analysis/tests/ to run."""
    # List of test folders
    test_folders = [f for f in os.scandir(TEST_PATH) if f.is_dir()]

    # Print test folders
    print("Available Test Folders\n=================")
    for i in range(len(test_folders)):
        print('{}: {}'.format(i, test_folders[i].name))

    if len(argv) == 2:
        index = int(argv[1])
    else:

        # Seek input for which test batch to run
        index = input("Enter Test Batch ")
        while not index.isdigit() or not 0 <= int(index) < len(test_folders):
            index = input("Try again: ")

    # Update global variable
    global batch_path
    batch_path = test_folders[int(index)].path


def get_tests():
    """Return JSON object of all tests."""
    with open(batch_path + '/tests.csv') as f:
        reader = list(csv.DictReader(f))
        tests = json.loads(json.dumps(reader))
    return [test for test in tests if int(test['RUN']) == 1]


def setup_test_batch():
    """Setup test batch folder for running tests."""
    # Check for existence of required files
    dir_contents = os.listdir(batch_path)
    for file in REQUIRED_FILES:
        if file not in dir_contents:
            print("\nERROR: No {} file in your test batch".format(file))
            return False

    # Check if this test batch has previously been run and ask for confirmation
    if len([i for i in REQUIRED_FOLDERS if i in dir_contents]) > 0:
        message = "This testbatch has been previously run. Would you like" + \
            " to wipe results and run tests again? (y/n): "
        response = input(message)
        while response not in ['y', 'Y', 'n', 'N']:
            response = input(message)
        if response in ['n', 'N']:
            return False

    # Create folders if they do not exist
    for folder in REQUIRED_FOLDERS:
        if folder in dir_contents:
            shutil.rmtree(batch_path + '/' + folder)
        os.mkdir(batch_path + '/' + folder)
    return True


def setup_test(test):
    """Edit params.h to setup values for test."""
    print("    Setting Up Test")
    # Find params.h file to edit
    file_path = str(Path(DIR_PATH).parent) + '/src/params.h'

    # Create Test Report Folder
    os.mkdir(batch_path + '/reports/' + test['NAME'])

    # Write parameters to params.h
    with open(file_path, 'w') as f:
        for param in PARAMS:
            line = "#define " + param + ' ' + str(test[param]) + '\n'
            f.write(line)

    # Setup OGM file
    # with open(batch_path + '/template.txt', 'r') as f:
    template = test['TEMPLATE']
    call("cd {}; python3 src/setup.py {} >/dev/null 2>/dev/null;".format(
         str(Path(DIR_PATH).parent), template))


def run_rrt(test_name):
    """Run Vtune Collect Hotspots function."""
    print("    Running RRT")
    # Find this path's parent directory
    path = Path(DIR_PATH)

    # Specify Result Directory Name
    result_dir = batch_path + '/results/' + test_name

    # Specify Log file path
    log_path = '{}/logs/{}.out'.format(batch_path, test_name)
    # Command to execute bash script
    command = 'cd {}; ./runRRT.bash {} {} > {} 2>&1;'.format(
        DIR_PATH, path.parent, result_dir, log_path)
    # Execute command
    call(command)


def run_tests(tests):
    """Setup all tests and collect hotspot analysis."""
    # Iterate through tests
    for test in tests:
        print("Running Test {}: {}".format(test['TESTNUM'], test['NAME']))
        setup_test(test)
        run_rrt(test['NAME'])
        copy_cache(test['NAME'])
        # graph_rrt(batch_path + '/reports/' + test['NAME'] + '/RRT.png')


def copy_cache(test_name):
    """Copy cache folder."""
    src = str(Path(DIR_PATH).parent) + '/src/cache'
    dst = batch_path + '/reports/' + test_name
    shutil.copytree(src, dst + '/cache')

    # Move performance.csv up one dir
    shutil.move(dst + '/cache/performance.csv', dst)


def graph_rrt(path):
    """Make 3D graph of RRT."""
    print("    Saving RRT Graph")
    call("cd ..; python3 {}/src/graph.py {} >/dev/null 2>/dev/null;".format(
        str(Path(DIR_PATH).parent), path))


def collect_results(tests):
    """Compile Data from reports of all tests."""
    for test in tests:

        # PERFORMANCE DATA
        # ================

        # Init Results Dict for test
        test['results'] = {f: 0. for f in ALLFUNCTIONS}

        # Path to Tests report folder
        reports_path = batch_path + '/reports/' + test['NAME']

        # Iterate through each method of collecting data
        report_path = reports_path + '/performance.csv'

        # Read relevant data from report file
        with open(report_path) as r:
            reader = list(csv.DictReader(r))
            for row in json.loads(json.dumps(reader)):
                func = row['Function Stack'].strip()
                if func in ALLFUNCTIONS:
                    test['results'][func] = float(row["CPU Time:Self"])

    return tests


def compile_data(tests):
    """Organise data into x and y(series)."""
    xname = 'XDIM'
    data = {}
    data['x'] = [test[xname] for test in tests]
    data['ys'] = {
        function: [test['results'][function] for test in tests]
        for function in FUNCTIONS}
    return data


def normalise(lst, denoms):
    """Normalise a list of values by dividing by sum of list."""
    return [lst[i] / denoms[i] for i in range(len(lst))]


def graph_reports(data):
    """Save graphs of data."""
    x = list(map(int, data['x']))
    ys = data['ys']

    fig = Figure(
        title="RRT Execution Profile",
        subtitle="Share of Fastest Possible Execution Time " +
                 "Per Function For Given Map Sizes",
        xlabel="Map Size (units)",
        ylabel="Share of CPU Execution Time (%)"
    )

    # Normalise values (weight by percentage of total time)
    ys_vals = list(ys.values())
    denoms = np.zeros(len(ys_vals[0]))
    for i in range(len(ys_vals)):
        for j in range(len(ys_vals[i])):
            denoms[j] += ys_vals[i][j]

    # weighted_ys = []
    for fun, time in ys.items():
        # weighted_ys.append(normalise(ys_vals[i], denoms))
        normalised = normalise(time, denoms)
        fig.add_series(
            x, normalised, label=fun)

    fig.plot(mode='stack')

    fig.set_axis_limit(y=(0, 1))
    fig.set_axis_ticks(x=x)

    x_tick_labels = [
        "{}".format(test['XDIM'], test['XDIM'], test['XDIM'])
        for test in tests]
    y_tick_labels = np.arange(0, 101, 10)
    fig.set_axis_tick_labels(x=x_tick_labels, y=y_tick_labels)

    fig.add_legend()
    fig.save(batch_path + "/graphs/performance")


def compile_success_rates(tests):
    """Compile success rates of each test into csv."""
    outfile = open(batch_path + '/success_rate.csv', 'w')
    writer = csv.writer(outfile)

    with open(batch_path + '/tests.csv') as f:
        reader = csv.reader(f)
        writer.writerow(next(reader) + ['SUCCESS_RATE'] + ['AVG_CPU_TIME'])
        for row in reader:
            with open(batch_path + '/reports/' +
                      row[2] + '/cache/success.txt') as s:
                writer.writerow(list(row) + [s.read()] +
                                [tests[int(row[0])]['results']['rrt']])


def plot_success_rates(tests):
    """Plot Success Rates."""
    # with open('success_rates.csv', 'r') as f:
    #     reader = csv.reader(f)
    #     for row in reader:
    fig = Figure(
        title='RRT Success Rates',
        xlabel=r'Average Execution Time ($\mu$s)',
        ylabel='Average Success Rate (%)'
    )

    fig.add_series(
        xs=range(0, 10),
        ys=[0, 2, 4, 6, 8, 10, 12, 14, 16, 18],
        label='test')

    plt.show()


# def plot_timing():
#     """Plot Line Graph of time.

#     This is fucking awful code, comment out so it doesnt break shit.
#     """
#     fig = Figure(
#         title="RRT Total Execution Time",
#         subtitle="Total Time For RRT To Complete Execution " +
#                  "Under Optimal Parameters for Given Map Sizes",
#         xlabel="Map Size (units)",
#         ylabel="Total Execution Time (seconds)"
#     )

#     test_folders = [f for f in os.scandir(TEST_PATH) if f.is_dir()]

#     global batch_path
#     batch_path = test_folders[2].path

#     tests = collect_results(get_tests())
#     data = compile_data(tests)

#     x = list(map(int, data['x']))
#     ys = data['ys']

#     y = np.zeros(5)
#     for fun, times in ys.items():
#         for i in range(len(times)):
#             y[i] += times[i]

#     fig.ax.plot(x, y, label="2D", color=COLORS[1])

#     batch_path = test_folders[5].path

#     tests = collect_results(get_tests())
#     data = compile_data(tests)

#     x = list(map(int, data['x']))
#     ys = data['ys']

#     y = np.zeros(5)
#     for fun, times in ys.items():
#         for i in range(len(times)):
#             y[i] += times[i]

#     fig.ax.plot(x, y, label="3D", color=COLORS[4])


#     fig.set_axis_limit(x=(min(x),max(x)), y=(0, int(max(y)+1)))
#     fig.set_axis_ticks(x=x)
    
#     x_tick_labels = [
#         "{}".format(test['XDIM'], test['XDIM'], test['XDIM'])
#         for test in tests]
#     y_tick_labels = np.arange(0,int(max(y)+1)+1,0.5) 
#     fig.set_axis_tick_labels(x=x_tick_labels, y=y_tick_labels)

#     fig.add_legend()
#     fig.save(batch_path + "/graphs/performance")


if __name__ == '__main__':
    # Choose Test Batch and save result to global variable

    choose_test_batch(sys.argv)

    tests = get_tests()
    if setup_test_batch():
        run_tests(tests)

    tests = collect_results(tests)
    data = compile_data(tests)
    graph_reports(data)
    compile_success_rates(tests)
    # plot_success_rates(tests)
