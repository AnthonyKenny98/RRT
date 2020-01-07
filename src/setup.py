"""Setup."""
# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-05 11:05:27
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-07 08:53:34

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
            (params['YDIM'] % params['RESOLUTION'] != 0) or \
            (params['ZDIM'] % params['RESOLUTION'] != 0):
        print(colored("WARNING: Resolution Dimension mismatch", 'red'))

    return params


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
    zlen = len(list(reader)[0][0].replace(';', ''))

    # Take Resolution into account
    XDIM = params['XDIM'] / params['RESOLUTION']
    YDIM = params['YDIM'] / params['RESOLUTION']
    ZDIM = params['ZDIM'] / params['RESOLUTION']

    # Check dimensions are compatible
    if XDIM % xlen != 0:
        print(colored("WARNING: Incompatible XDIM between template ({}) "
                      "& graph ({})".format(xlen, params['XDIM']), 'red'))
    if YDIM % ylen != 0:
        print(colored("WARNING: Incompatible YDIM between template ({}) "
                      "& graph ({}))".format(ylen, params['YDIM']), 'red'))
    if ZDIM % zlen != 0:
        print(colored("WARNING: Incompatible ZDIM between template ({}) "
                      "& graph ({}))".format(zlen, params['ZDIM']), 'red'))

    # Calculate multiplier factors for each dimension
    x_multiplier = int(XDIM / xlen)
    y_multiplier = int(YDIM / ylen)
    z_multiplier = int(ZDIM / zlen)

    # Open template file to read from
    infile = open(template, 'r')
    reader = csv.reader(infile)

    # Open OGM file to write to
    outfile = open('cache/ogm.csv', 'w')
    writer = csv.writer(outfile, delimiter=',')

    # Iterate through template
    for row in reader:

        # Write each row <Y_MULTIPLIER> times
        for y in range(y_multiplier):
            new_row = []

            # For each grid in X-Y Plane
            for col in row:

                # Rescale contentes of cell (Z-axis seperated by ';')
                new_col = [j for i in[[z] * z_multiplier for z in
                           col.replace(';', '')] for j in i]
                new_col = ';'.join(new_col)
                new_row.append(new_col)

            # Rescale X axis and write row to outfile
            writer.writerow(j for i in [[x] * x_multiplier
                            for x in new_row] for j in i)

    # Close files
    infile.close()
    outfile.close()


if __name__ == '__main__':
    params = rrt_config()
    template = choose_template()
    resize_template(params, template)
