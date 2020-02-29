"""Run python analysis using Intel Vtune software."""

# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-02 09:44:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-02-28 22:53:54

import csv
import json
from matplotlib import pyplot as plt
import numpy as np
import os
from pathlib import Path
import subprocess
import shutil

# Path in project of this file
DIR_PATH = os.path.dirname(os.path.realpath(__file__))
# Path for all test folders
TEST_PATH = DIR_PATH + '/tests'
# Path for test batch - updated in choose_test_batch()
batch_path = ''

# Required files and folders for each test batch
REQUIRED_FILES = ['template.txt', 'tests.csv']
REQUIRED_FOLDERS = ['results', 'logs', 'reports', 'graphs']

# Parameters for params.h
PARAMS = ['XDIM', 'YDIM', 'ZDIM',
          'EPSILON', 'NUM_NODES', 'RESOLUTION', 'NUMBUCKETS']

# Functions that we want to measure
FUNCTIONS = [
    # 'benchmark',
    'getRandomNode',
    'findNearestNode',
    'stepFromTo',
    'pointCollisions',
    'edgeCollisions'
]

# Testing Methods
METHODS = ['vtune', 'performance']

COLORS = ['#999999', '#4285F4', '#34A853', '#FBBC05', '#EA4335']


def call(command):
    """Wrap subprocess call for bash use."""
    subprocess.call(command, shell=True, executable='/bin/bash')


def choose_test_batch():
    """Choose with test batch under analysis/tests/ to run."""
    # List all test folders
    test_folders = [f for f in os.scandir(TEST_PATH) if f.is_dir()]

    # Seek input for which test batch to run
    print("Available Test Folders\n=================")
    for i in range(len(test_folders)):
        print('{}: {}'.format(i, test_folders[i].name))
    index = input("Enter the index number of the test batch "
                  "you would like to run: ")
    while not index.isdigit() or not 0 <= int(index) < len(test_folders):
        index = input("Index must be a valid integer. Try again: ")

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
    """Setup test batch folder for running tests.

    + Check for required files
    + Check if test batch has previously been run
    + Setup required directory structure.
    """
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
    with open(batch_path + '/template.txt', 'r') as f:
        template = f.read()
        call("cd {}; python3 src/setup.py {} >/dev/null 2>/dev/null;".format(
             str(Path(DIR_PATH).parent), template))


def collect_hotspots(test_name):
    """Run Vtune Collect Hotspots function."""
    print("    Collecting Hotspots")
    # Find this path's parent directory
    path = Path(DIR_PATH)

    # Specify Result Directory Name
    result_dir = batch_path + '/results/' + test_name

    # Specify Log file path
    log_path = '{}/logs/{}.out'.format(batch_path, test_name)
    # Command to execute bash script
    command = 'cd {}; ./collectHotspots.bash {} {} > {} 2>&1;'.format(
        DIR_PATH, path.parent, result_dir, log_path)
    # Execute command
    call(command)


def topdown_report(test_name):
    """Run Vtune Top-Down Report."""
    print("    Running TopDown Report")
    path = Path(DIR_PATH)

    # Specify Result Directory Name
    result_dir = batch_path + '/results/' + test_name

    # Specify Report Directory Name
    report_dir = batch_path + '/reports/' + test_name + '/vtune'

    # Specify Log file path
    log_path = '{}/logs/{}.out'.format(batch_path, test_name)

    # Command to execute bash script
    command = 'cd {}; ./topDownReport.bash {} {} {} >> {} 2>&1;'.format(
        DIR_PATH, path.parent, result_dir, report_dir, log_path)
    # Execute command
    call(command)


def run_tests():
    """Setup all tests and collect hotspot analysis."""
    # Iterate through tests
    for test in get_tests():
        print("Running Test {}: {}".format(test['TESTNUM'], test['NAME']))
        setup_test(test)
        collect_hotspots(test['NAME'])
        topdown_report(test['NAME'])
        copy_cache(test['NAME'])
        graph_rrt(batch_path + '/reports/' + test['NAME'] + '/RRT.png')


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


def compile_report_data():
    """Compile Data from reports of all tests."""
    tests = get_tests()
    for test in tests:

        # Init Results Dict for test
        test['results'] = {f: {x: 0. for x in METHODS} for f in FUNCTIONS}

        # Path to Tests report folder
        reports_path = batch_path + '/reports/' + test['NAME']

        # Iterate through each method of collecting data
        for m in METHODS:
            report_path = reports_path + '/' + m + '.csv'

            # Read relevant data from report file
            with open(report_path) as r:
                reader = list(csv.DictReader(r))
                for row in json.loads(json.dumps(reader)):
                    func = row['Function Stack'].strip()
                    if func in FUNCTIONS:
                        test['results'][func][m] = float(row["CPU Time:Self"])

    # Organise data into x and y(series)
    xname = 'NUM_NODES'
    data = {m: {} for m in METHODS}
    for m in METHODS:
        data[m]['x'] = [test[xname] for test in tests]
        data[m]['ys'] = {
            function: [test['results'][function][m] for test in tests]
            for function in FUNCTIONS}
    # print(str(data))
    return data


def normalise(lst, denoms):
    """Normalise a list of values by dividing by sum of list."""
    return [lst[i] / denoms[i] for i in range(len(lst))]


def graph_reports(data):
    """Save graphs of data."""
    for method in METHODS:
        x = data[method]['x']
        ys = data[method]['ys']

        plt.figure(figsize=(15, 10))
        plt.title("RRT Performance Breakdown")
        plt.xlabel("Number of Nodes in Graph")
        plt.ylabel("% of CPU Time")
        plt.grid(color='gray', axis='y')

        # Normalise values (weight by percentage of total time)
        ys_vals = list(ys.values())
        denoms = np.zeros(len(ys_vals[0]))
        for i in range(len(ys_vals)):
            for j in range(len(ys_vals[i])):
                denoms[j] += ys_vals[i][j]

        weighted_ys = []
        for i in range(len(ys_vals)):
            weighted_ys.append(normalise(ys_vals[i], denoms))

        plt.stackplot(x, weighted_ys,
                      colors=COLORS,
                      labels=ys.keys())
        plt.legend()
        plt.savefig(batch_path + "/graphs/" + method)


if __name__ == '__main__':
    print("WARNING: Have you remembered to set ptrace_scope = 0?")
    choose_test_batch()
    if setup_test_batch():
        run_tests()
    data = compile_report_data()
    graph_reports(data)
