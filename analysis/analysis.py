"""Run python analysis using Intel Vtune software."""

# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-02 09:44:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-02 13:59:29

import subprocess
import os
from pathlib import Path
import csv
import json


dir_path = os.path.dirname(os.path.realpath(__file__))


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


if __name__ == '__main__':
    run_reports()
