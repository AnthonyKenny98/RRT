"""Setup."""
# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-05 11:05:27
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-05 14:24:01

import csv
import os
from termcolor import colored


def rrt_config():
    """Get configuration params from params.h file."""
    params = {}

    # Read params.h
    with open('params.h', 'r') as file:
        for line in file:
            params[line.split(' ')[1]] = int(line.split(' ')[2].strip('\n'))

    # Check X and Y Dimension of config are integer divisible by resolution
    if (params['XDIM'] % params['RESOLUTION'] != 0) or \
            (params['YDIM'] % params['RESOLUTION'] != 0):
        print(colored("WARNING: Resolution Dimension mismatch", 'red'))

    return params


def create_blank_space(params):
    """Create new csv representing completely empty space."""
    with open('ogm.csv', 'w') as ogm:
        writer = csv.writer(ogm, delimiter=',')
        for i in range(int(params['XDIM'] / params['RESOLUTION'])):
            writer.writerow(
                [0] * int(params['YDIM'] / params['RESOLUTION']))

    print(colored("Blank OGM created. Must edit before running RRT", 'green'))


def choose_template():
    """Return name of template to use."""
    templates = os.listdir('templates')
    print("Available Templates\n===============")
    for i in range(len(templates)):
        print("{}: {}".format(i, templates[i]))
    index = input("Enter the index number of the space template "
                  "you would like to use: ")
    while not index.isdigit() or not 0 <= int(index) < len(templates):
        index = input(colored(
            "Index must be a valid integer. Try again: ", 'red'))
    return 'templates/' + templates[int(index)]


def resize_template(params, template):
    """Resize Template according to params."""
    # Open file
    with open(template, 'r') as f:
        reader = list(csv.reader(f))

    # Get dimensions of template
    xlen = len(list(reader)[0])
    ylen = len(list(reader))

    # Take Resolution into account
    XDIM = params['XDIM'] / params['RESOLUTION']
    YDIM = params['YDIM'] / params['RESOLUTION']

    # Check dimensions are compatible
    if XDIM % xlen != 0:
        print(colored("WARNING: Incompatible XDIM between template ({}) "
                      "& graph ({})".format(xlen, params['XDIM'], 'red')))
    if YDIM % ylen != 0:
        print(colored("WARNING: Incompatible YDIM between template ({}) "
                      "& graph ({}))".format(ylen, params['YDIM'], 'red')))

    # Resize and write
    x_multiplier = int(XDIM / xlen)
    y_multiplier = int(YDIM / ylen)
    with open(template, 'r') as f:
        reader = csv.reader(f)
        with open('ogm.csv', 'w') as ogm:
                writer = csv.writer(ogm, delimiter=',')
                for row in reader:
                    for y in range(y_multiplier):
                        writer.writerow(j for i in [[x] * x_multiplier
                                        for x in row] for j in i)

if __name__ == '__main__':
    params = rrt_config()
    template = choose_template()
    resize_template(params, template)
