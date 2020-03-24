"""LEarning."""
# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-05 20:46:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-03-24 14:55:29

import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
import matplotlib.colors as mcol
from mpl_toolkits.mplot3d.art3d import Poly3DCollection, Line3DCollection
import numpy as np
import csv
import sys
import os

DIR_PATH = os.path.dirname(os.path.realpath(__file__))
PARAM_PATH = 'params.h'
OGM_PATH = 'cache/ogm.csv'
COLORS = '../../aux/colors/'


def rrt_config():
    """Get configuration params from params.h file."""
    params = {}
    with open(DIR_PATH + '/params.h', 'r') as file:
        for line in file:
            params[line.split(' ')[1]] = int(line.split(' ')[2].strip('\n'))
    return params


def plot_2d():
    """Plot in 2D."""
    colors = []

    for i in range(1, 5):
        with open(COLORS + str(i) + '.txt') as f:
            colors.append(f.read())

    def get_ogm(params):
        """Return 2D numpy array of OGM."""
        ogm = np.ndarray(shape=(params['XDIM'], params['YDIM']))
        with open(OGM_PATH, 'r') as f:
            reader = list(csv.reader(f))
            for row in range(len(reader)):
                for cell in range(len(reader[row])):
                    ogm[row][cell] = reader[row][cell][0]
        return ogm

    params = rrt_config()
    ogm = get_ogm(params)

    # rescale ogm
    # ogm = np.kron(ogm, np.ones((params['RESOLUTION'], params['RESOLUTION'])))

    # New figure
    fig = plt.figure()
    ax = fig.add_subplot(
        111,
        xlim=(0, params['XDIM']),
        ylim=(0, params['YDIM'])
    )

    # Define Color Map
    cmap = mcol.ListedColormap(['white', colors[2]])

    # Build Grid Map
    ax.imshow(ogm, cmap=cmap, extent=(0, params['XDIM'], params['YDIM'], 0),
              origin='lower')

    # Set major ticks
    ax.set_xticks(np.arange(0, params['XDIM'], params['RESOLUTION']))
    ax.set_yticks(np.arange(0, params['YDIM'], params['RESOLUTION']))

    # Disable major tick labels
    ax.set_xticklabels([])
    ax.set_yticklabels([])

    # Set grid based on major ticks
    ax.grid(color='grey', linestyle='-', linewidth=0.5)

    # Set minor ticks for scale
    ax.set_xticks(np.arange(0, params['XDIM'], 10), minor=True)
    ax.set_yticks(np.arange(0, params['YDIM'], 10), minor=True)

    # Labels for minor ticks
    ax.set_xticklabels(np.arange(0, params['XDIM'], 10), minor=True)
    ax.set_yticklabels(np.arange(0, params['YDIM'], 10), minor=True)

    # Plot Edges
    print("Plotting Edges")
    edges = []
    with open('cache/path.txt', 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            edges.append(((float(row[0]), float(row[1])),
                         (float(row[2]), float(row[3]))))

    collection = LineCollection(edges, linewidth=1, color=colors[0])
    ax.add_collection(collection)

    # Plot start point
    print("Plotting Start Point")
    with open('cache/startPoint.txt', 'r') as f:
        point = list(csv.reader(f))[0]
    ax.scatter(float(point[0]), float(point[1]),
               color=colors[3], marker='*', s=100)

    plt.figtext(.5, .93, 'RRT', fontsize=18, ha='center')
    plt.figtext(
        .5, .89,
        '{} Points in a {} x {} workspace'.format(
            params["NUM_CONFIGS"], params["XDIM"], params["YDIM"]
        ),
        fontsize=10, ha='center')
    plt.savefig("RRTGraph.png")


def plot_3d(argv):
    """."""
    print("Retrieving RRT Parameters")
    params = rrt_config()

    def get_ogm(params):
        """Get OGM."""
        ogm = np.zeros(
            (params['XDIM'], params['YDIM'], params['ZDIM']),
            dtype=int)
        with open(DIR_PATH + '/cache/ogm.csv', 'r') as f:
            reader = list(csv.reader(f))
            for x in range(int(params['XDIM'])):
                row = reader[x]
                for y in range(int(params['YDIM'])):
                    col = row[y]
                    for z in range(int(params['ZDIM'])):
                        elem = col.split(';')[z]
                        ogm[x][y][z] = elem
        return ogm

    # Create Figure
    print("Initializing Plot")
    fig = plt.figure("Configuration Space")

    # Init plot in 3D with correct axis limits
    ax = fig.add_subplot(
        111, projection='3d',
        xlim=(0, params['XDIM']),
        ylim=(0, params['YDIM']),
        zlim=(0, params['ZDIM']))

    # Plot start point
    print("Plotting Start Point")
    with open(DIR_PATH + '/cache/startPoint.txt', 'r') as f:
        point = list(csv.reader(f))[0]
    ax.scatter(float(point[0]), float(point[1]), float(point[2]),
               color='red', marker='*')

    # Plot goal point
    print("Plotting Goal Point")
    with open(DIR_PATH + '/cache/goalPoint.txt', 'r') as f:
        point = list(csv.reader(f))[0]
    ax.scatter(float(point[0]), float(point[1]), float(point[2]),
               color='green', marker='*')

    # Plot Edges
    print("Plotting Edges")
    edges = []
    with open(DIR_PATH + '/cache/path.txt', 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            edges.append(((float(row[0]), float(row[1]), float(row[2])),
                         (float(row[3]), float(row[4]), float(row[5]))))

    collection = Line3DCollection(edges, linewidth=1, color="blue")
    ax.add_collection3d(collection)

    # Get Occupancy Grid Map from RRT
    print("Retrieving OGM from RRT Data")
    ogm = get_ogm(params)

    def plot_prism(origin, s, ax):
        """."""
        add = np.array([[0, 0, 0],
                        [0, s, 0],
                       [s, s, 0],
                       [s, 0, 0],
                       [0, 0, s],
                       [0, s, s],
                       [s, s, s],
                       [s, 0, s]])

        p = origin + add

        def draw_face(p1):
            x = p1[:, 0]
            y = p1[:, 1]
            z = p1[:, 2]

            verts = [list(zip(x, y, z))]

            collection = Poly3DCollection(
                verts, linewidths=1, edgecolors='black',
                zsort='min', antialiased=False)
            face_color = "red"
            collection.set_facecolor(face_color)
            ax.add_collection3d(collection)

        draw_face(np.array([p[0], p[1], p[2], p[3]]))
        draw_face(np.array([p[4], p[5], p[6], p[7]]))
        draw_face(np.array([p[0], p[1], p[5], p[4]]))
        draw_face(np.array([p[2], p[3], p[7], p[6]]))
        draw_face(np.array([p[0], p[3], p[7], p[4]]))
        draw_face(np.array([p[1], p[2], p[6], p[5]]))

    print("Rendering Occupied Grids")
    # Draw Occupied Grids
    for i in range(int(params['XDIM'])):
        for j in range(int(params['YDIM'])):
            for k in range(int(params['ZDIM'])):
                if ogm[i][j][k]:
                    origin = [j, i, k]
                    plot_prism(origin, params['RESOLUTION'], ax)

    # Update Viewing Angle
    print("Finalizing")
    ax.view_init(elev=10, azim=90)

    ax.set_xlabel("X Axis")
    ax.set_ylabel("Y Axis")
    ax.set_zlabel("Z Axis")

    # Show Plot
    if len(argv) != 2:
        plotname = "RRTGraph.png"
    else:
        plotname = argv[1]
    plt.savefig(plotname)

    print("Done")

if __name__ == "__main__":
    with open(DIR_PATH + '/cache/startPoint.txt', 'r') as f:
        point = list(csv.reader(f))[0]
    if len(point) == 2:
        plot_2d()
    else:
        plot_3d(sys.argv)
