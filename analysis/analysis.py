"""Run python analysis using Intel Vtune software."""

# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-02 09:44:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-05 11:16:19

import csv
import json
from matplotlib import pyplot as plt
import os
from pathlib import Path
import subprocess

# Path in project of this file
DIR_PATH = os.path.dirname(os.path.realpath(__file__))
# Path for all test folders
TEST_PATH = DIR_PATH + '/tests'
# Path for test batch - updated in choose_test_batch()
test_batch_path = ''

# Subroutines of note
subroutines = ['findNearestNode', '_point_collision', 'edgeCollisions']


def call(command):
    """Wrap subprocess call for bash use."""
    subprocess.call(command, shell=True, executable='/bin/bash')


def choose_test_batch():
    """Run preliminaries."""
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
    global test_batch_path
    test_batch_path = test_folders[int(index)].path


def setup_test_batch():
    """Setup test batch folder for running tests."""
    # Check for existence of tests.csv file
    if 'tests.csv' not in os.listdir(test_batch_path):
        print("\nERROR: No tests.csv file in your test batch")
        return False

    # List of required folders
    folders = ['results', 'logs', 'reports', 'graphs']

    # Create folders if they do not exist
    for folder in folders:
        if folder not in os.listdir(test_batch_path):
            os.mkdir(test_batch_path + '/' + folder)
    return True


def match_tests(folder):
    """Check that the entries in a folder match the number of tests."""
    return len(os.listdir(test_batch_path + folder)) == len(get_tests())


def setup_test(test):
    """Edit params.h to setup values for test."""
    # Find params.h file to edit
    file_path = str(Path(DIR_PATH).parent) + '/src/params.h'

    # "define" keywords
    keywords = ['NUM_OBSTACLES', 'OBSTACLE_SIZE', 'XDIM', 'YDIM',
                'EPSILON', 'NUM_NODES', 'RESOLUTION']

    with open(file_path, 'w') as f:
        for keyword in keywords:
            line = "#define " + keyword + ' ' + str(test[keyword]) + '\n'
            f.write(line)


def collect_hotspots(test_name):
    """Run Vtune Collect Hotspots function."""
    # Find this path's parent directory
    path = Path(DIR_PATH)

    # Specify Result Directory Name
    result_dir = test_batch_path + '/results/' + test_name

    # Specify Log file path
    log_path = '{}/logs/{}.out'.format(test_batch_path, test_name)
    # Command to execute bash script
    command = 'cd {}; ./collectHotspots.bash {} {} > {} 2>&1;'.format(
        DIR_PATH, path.parent, result_dir, log_path)
    # Execute command
    call(command)


def topdown_report(test_name):
    """Run Vtune Top-Down Report."""
    path = Path(DIR_PATH)

    # Specify Result Directory Name
    result_dir = test_batch_path + '/results/' + test_name

    # Specify Report Directory Name
    report_dir = test_batch_path + '/reports/' + test_name

    # Specify Log file path
    log_path = '{}/logs/{}.out'.format(test_batch_path, test_name)

    # Command to execute bash script
    command = 'cd {}; ./topDownReport.bash {} {} {} >> {} 2>&1;'.format(
        DIR_PATH, path.parent, result_dir, report_dir, log_path)
    # Execute command
    call(command)


def get_tests():
    """Return JSON object of all tests."""
    with open(test_batch_path + '/tests.csv') as f:
        reader = list(csv.DictReader(f))
        tests = json.loads(json.dumps(reader))
    return [test for test in tests if int(test['RUN']) == 1]


def run_tests():
    """Setup all tests and collect hotspot analysis."""
    for test in get_tests():
        print("Running Test {}: {}".format(test['TESTNUM'], test['NAME']))
        setup_test(test)
        collect_hotspots(test['NAME'])


def run_reports():
    """Run all reports."""
    for test in get_tests():
        print("Running Report {}: {}".format(test['TESTNUM'], test['NAME']))
        topdown_report(test['NAME'])


def compile_report_data():
    """Compile Data from reports of all tests."""
    tests = get_tests()
    for test in tests:
        test["results"] = {}

        # Add results to tests
        report_path = test_batch_path + '/reports/' + test['NAME'] + '.csv'
        with open(report_path, 'r') as f:
            reader = list(csv.DictReader(f))
            for row in json.loads(json.dumps(reader)):
                if row['Function Stack'].strip() in subroutines:
                    test['results'][row["Function Stack"].strip()] = float(
                        row["CPU Time:Total"]) / 100

    # Organise data
    obstacles = {}
    for test in tests:
        if int(test['NUM_OBSTACLES']) not in obstacles.keys():
            obstacles[int(test['NUM_OBSTACLES'])] = {}
        obstacles[int(test['NUM_OBSTACLES'])][
            int(test['NUM_NODES'])] = test['results']
    return obstacles


def graph_reports(obstacles):
    """Save graphs of data."""
    for num_obstacles in obstacles.keys():
        x = []
        y = {s: [] for s in subroutines}
        for key, value in obstacles[num_obstacles].items():
            x.append(key)
            for subroutine in subroutines:
                y[subroutine].append(value.get(subroutine, 0))

        plt.figure(figsize=(15, 10))
        plt.suptitle("% of CPU Time per Function")
        plt.title("{} Obstacles".format(num_obstacles))
        plt.xlabel("Number of Nodes in Graph")
        plt.ylabel("% of CPU Time")
        plt.grid(color='gray', axis='y')
        plt.xlim(min(x), max(x))

        plt.stackplot(x, y.values(),
                      colors=["#ea4335", "#4285f4", "#fbbc04"],
                      labels=['findNearestNode', 'point_collision',
                              'edgeCollisions'])
        plt.legend()
        plt.savefig(test_batch_path + "/graphs/" + str(num_obstacles) + "obs")


if __name__ == '__main__':
    choose_test_batch()
    if setup_test_batch():
        if not match_tests('/results'):
            run_tests()
        if not match_tests('/reports'):
            run_reports()
    obstacles = compile_report_data()
    graph_reports(obstacles)
