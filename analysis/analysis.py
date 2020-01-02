"""Run python analysis using Intel Vtune software."""

# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-02 09:44:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-02 12:45:32

import subprocess
import os
from pathlib import Path
import csv
import json


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
    dir_path = os.path.dirname(os.path.realpath(__file__))
    path = Path(dir_path)

    # Specify Result Directory Name
    result_dir = dir_path + '/results/' + test_name

    # Command to execute bash script
    command = './collectHotspots.bash {} {} > logs/{}.out 2>&1'.format(
        path.parent, result_dir, test_name)
    # Execute command
    call(command)

# Parse all Tests
with open('tests.csv') as f:
    reader = csv.DictReader(f)
    # tests = [json.dumps(row) for row in reader]
    rows = list(reader)
    tests = json.loads(json.dumps(rows))

# Run Tests
for test in tests:
    if int(test['RUN']):
        print("Running Test: {} - {}".format(test['TESTNUM'], test['NAME']))
        setup_test(test)
        collect_hotspots(test['NAME'])
