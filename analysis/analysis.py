"""Run python analysis using Intel Vtune software."""

# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-02 09:44:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-02 11:11:11

import subprocess
import os
from pathlib import Path

tests = [
    {
        'test': 0,
        'name': 'test1',
        'NUM_OBSTACLES': 10,
        'OBSTACLE_SIZE': 10,
        'XDIM': 200,
        'YDIM': 200,
        'EPSILON': 20,
        'NUM_NODES': 200,
        'RESOLUTION': 3
    }
]


def call(command):
    """Wrap subprocess call for bash use."""
    subprocess.call(command, shell=True, executable='/bin/bash')


def setup_test(test):
    """Edit params.h to setup values for test."""
    # Find file
    dir_path = os.path.dirname(os.path.realpath(__file__))
    file_path = str(Path(dir_path).parent) + '/src/'
    original_file_path = file_path + 'params.h'
    new_file_path = file_path + 'temp_params.h'

    # "define" keywords
    keywords = ['NUM_OBSTACLES', 'OBSTACLE_SIZE', 'XDIM', 'YDIM',
                'EPSILON', 'NUM_NODES', 'RESOLUTION']

    newfile = open(new_file_path, 'w')

    with open(original_file_path, 'r') as original:
        for line in original:
            for keyword in keywords:
                if line.startswith("#define") and keyword in line:
                    line = "#define " + keyword + " " + str(test[keyword]) + "\n"
            newfile.write(line)
    newfile.close()

    os.remove(original_file_path)
    os.rename(new_file_path, original_file_path)


def collect_hotspots(test_name):
    """Run Vtune Collect Hotspots function."""
    # Find this path's parent directory
    dir_path = os.path.dirname(os.path.realpath(__file__))
    path = Path(dir_path)

    # Specify Result Directory Name
    result_dir = dir_path + '/results/' + test_name

    # Command to execute bash script
    command = './collectHotspots.bash {} {}'.format(path.parent, result_dir)
    # Execute command
    call(command)

for test in tests:
    # collect_hotspots(test['name'])
    setup_test(test)
