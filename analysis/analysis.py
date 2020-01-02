"""Run python analysis using Intel Vtune software."""

# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-02 09:44:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-02 18:01:05

import csv
import json
from matplotlib import pyplot as plt
import os
from pathlib import Path
import subprocess

# Path in project of this file
dir_path = os.path.dirname(os.path.realpath(__file__))
subroutines = ['findNearestNode', 'edgeCollisions', '_point_collision']


def call(command):
    """Wrap subprocess call for bash use."""
    subprocess.call(command, shell=True, executable='/bin/bash')


def setup_test(test):
    """Edit params.h to setup values for test."""
    # Find file
    dir_path = os.path.dirname(os.path.realpath(__file__))
    file_path = str(Path(dir_path).parent) + '/src/params.h'

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
    path = Path(dir_path)

    # Specify Result Directory Name
    result_dir = dir_path + '/results/' + test_name

    # Specify Log file path
    log_path = '{}/logs/{}.out'.format(dir_path, test_name)
    # Command to execute bash script
    command = 'cd {}; ./collectHotspots.bash {} {} > {} 2>&1;'.format(
        dir_path, path.parent, result_dir, log_path)
    # Execute command
    call(command)


def topdown_report(test_name):
    """Run Vtune Top-Down Report."""
    path = Path(dir_path)

    # Specify Result Directory Name
    result_dir = dir_path + '/results/' + test_name

    # Specify Report Directory Name
    report_dir = dir_path + '/reports/' + test_name

    # Command to execute bash script
    command = 'cd {}; ./topDownReport.bash {} {} {};'.format(
        dir_path, path.parent, result_dir, report_dir)
    # Execute command
    call(command)


def get_tests():
    """Return JSON object of all tests."""
    with open(dir_path + '/tests.csv') as f:
        reader = list(csv.DictReader(f))
        return json.loads(json.dumps(reader))


def run_tests():
    """Setup all tests and collect hotspot analysis."""
    for test in get_tests():
        if int(test['RUN']):
            print("Running Test: {}-{}".format(test['TESTNUM'], test['NAME']))
            setup_test(test)
            collect_hotspots(test['NAME'])


def run_reports():
    """Run all reports."""
    tests = [f.name for f in os.scandir(dir_path + '/results')
             if f.is_dir()]
    for test in tests:
        topdown_report(test)


def compile_report_data():
    """Compile Data from reports of all tests."""
    tests = get_tests()
    for test in tests:
        test["results"] = {}

        # Add results to tests
        report_path = dir_path + '/reports/' + test['NAME'] + '.csv'
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

        plt.stackplot(x, y.values(), colors=["#ea4335", "#4285f4", "#fbbc04"])
        plt.savefig(dir_path + "/graphs/" + str(num_obstacles) + "obs")


if __name__ == '__main__':
    if input("Would you like to re-run tests? Answering no will simply compile"
             "report data and make graphs. (y/n)") == 'y':
        print("yes")
        # run_tests()
        # run_reports()
    obstacles = compile_report_data()
    graph_reports(obstacles)
