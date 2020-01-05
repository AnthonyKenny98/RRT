"""Graph rrt from cache folder."""

# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-05 17:04:03
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-05 18:07:58


import matplotlib.pyplot as plt
import matplotlib.colors as mcol
import numpy as np


PARAM_PATH = 'params.h'
OGM_PATH = 'cache/ogm.csv'


def rrt_config():
    """Get configuration params from params.h file."""
    params = {}

    # Read params.h
    with open('params.h', 'r') as file:
        for line in file:
            params[line.split(' ')[1]] = int(line.split(' ')[2].strip('\n'))
    return params


def read_csv():
    """Return 2D numpy array of OGM."""
    with open(OGM_PATH, 'r') as f:
        ogm = np.loadtxt(f, delimiter=',')
    return ogm


params = rrt_config()
ogm = read_csv()

# rescale ogm
ogm = np.kron(ogm, np.ones((params['RESOLUTION'], params['RESOLUTION'])))

# New figure
plt.figure()

# Define Color Map
cmap = mcol.ListedColormap(['white', 'red'])

# Build Grid Map
im = plt.imshow(ogm, cmap=cmap)
ax = plt.gca()

# Set ticks
ax.set_xticks(np.arange(-.5, params['XDIM'], 10), minor=True)
ax.set_yticks(np.arange(-.5, params['YDIM'], 10), minor=True)

# Labels for ticks
ax.set_xticklabels(np.arange(0, params['XDIM'], 10), minor=True)
ax.set_yticklabels(np.arange(0, params['YDIM'], 10), minor=True)

# Gridlines based on ticks
# Set ticks
ax.set_xticks(np.arange(-.5, params['XDIM'], params['RESOLUTION']))
ax.set_yticks(np.arange(-.5, params['YDIM'], params['RESOLUTION']))
ax.set_xticklabels([])
ax.set_yticklabels([])
ax.grid( color='grey', linestyle='-', linewidth=0.5)

plt.show()
