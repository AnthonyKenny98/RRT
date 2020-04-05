"""Graphing output of RRT."""
# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-05 20:46:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-04-05 21:11:48

import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
import matplotlib.colors as mcol
from mpl_toolkits.mplot3d.art3d import Poly3DCollection, Line3DCollection
import numpy as np
import csv
import sys
import os

TK_SILENCE_DEPRECATION = 1
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
    params['DIM'] = int(params['XDIM'])
    return params


def get_colors():
    """Get colors from main project."""
    colors = []
    for i in range(1, 5):
        with open(COLORS + str(i) + '.txt') as f:
            colors.append(f.read())
    return colors


def plot_prism(origin, s, ax):
    """Plot a prism."""
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
        face_color = 'red'
        collection.set_facecolor(face_color)
        ax.add_collection3d(collection)

    draw_face(np.array([p[0], p[1], p[2], p[3]]))
    draw_face(np.array([p[4], p[5], p[6], p[7]]))
    draw_face(np.array([p[0], p[1], p[5], p[4]]))
    draw_face(np.array([p[2], p[3], p[7], p[6]]))
    draw_face(np.array([p[0], p[3], p[7], p[4]]))
    draw_face(np.array([p[1], p[2], p[6], p[5]]))


def get_ogm():
    """Load Occupancy Grid Map."""
    shape = [params['DIM']] * 2
    if _3D:
        shape.append(params['DIM'])
    ogm = np.ndarray(shape=shape)
    with open(OGM_PATH, 'r') as f:
            reader = list(csv.reader(f))
            for x in range(params['DIM']):
                for y in range(params['DIM']):
                    if (_3D):
                        for z in range(params['DIM']):
                            ogm[x][y][z] = reader[x][y].split(';')[z]
                    else:
                        ogm[x][y] = reader[x][y][0]
    return ogm


def plot_ogm(ax, ogm):
    """Plot the Occupancy Grid Map for a given dimension."""
    if _3D:
        for i in range(params['DIM']):
            for j in range(params['DIM']):
                for k in range(params['DIM']):
                    if ogm[i][j][k]:
                        origin = [j, i, k]
                        plot_prism(origin, params['RESOLUTION'], ax)
    else:
        # Define Color Map
        cmap = mcol.ListedColormap(['white', colors[2]])

        # Build Grid Map
        ax.imshow(ogm, cmap=cmap,
                  extent=(0, params['DIM'], params['DIM'], 0),
                  origin='lower')


def plot_point(ax, point, color, label=None):
    """Plot point."""
    if _3D:
        ax.scatter(float(point[0]), float(point[1]), float(point[2]),
                   color=color, marker='*', label=label)
    else:
        ax.scatter(float(point[0]), float(point[1]),
                   color=color, marker='*', s=200, zorder=10, label=label)


def plot_graph(ax, num_edges):
    """Plot Edges."""
    print('Plotting RRT Graph')
    edges = []
    with open('cache/path.txt', 'r') as f:
        reader = list(csv.reader(f))
        for i in range(1, num_edges + 1):
            try:
                row = reader[i]
            except Exception:
                break
            if _3D:
                edges.append(((float(row[0]), float(row[1]), float(row[2])),
                             (float(row[3]), float(row[4]), float(row[5]))))
            else:
                edges.append(((float(row[0]), float(row[1])),
                             (float(row[2]), float(row[3]))))

    if _3D:
        collection = Line3DCollection(edges, linewidth=0.5, color='blue')
        ax.add_collection3d(collection)
    else:
        collection = LineCollection(
            edges, linewidth=1, color=colors[0])
        ax.add_collection(collection)


def write_text(plt, num_edges):
    """Write Text for Figure."""
    height = .93 if _3D else .93
    fontsize = 14 if _3D else 14
    plt.figtext(.5, height, 'RRT', fontsize=24, ha='center')
    plt.figtext(
        .5, height - .04,
        r'K = {}, $\epsilon$ = {}, Goal Bias = {}'.format(
            num_edges, params['EPSILON'], params['GOAL_BIAS']
        ),
        fontsize=fontsize, ha='center')


def plot(num_edges):
    """Plot for (dim)D."""
    print('Loading Occupancy Grid Map')
    ogm = get_ogm()

    print('Initializing Plot')
    fig = plt.figure(
        figsize=(6, 6),
        dpi=300
    )

    # Setup axis options
    xlim = (0, params['DIM'])
    ylim = (0, params['DIM'])
    zlim = (0, params['DIM']) if _3D else None
    projection = '3d' if _3D else None

    if _3D:
        ax = fig.add_subplot(
            111,
            projection=projection,
            xlim=xlim,
            ylim=ylim,
            zlim=zlim
        )
    else:
        ax = fig.add_subplot(
            111,
            xlim=xlim,
            ylim=ylim
        )

    plot_ogm(ax, ogm)

    if not _3D:
        # Set major ticks
        major_ticks = np.arange(0.001, params['DIM'] + 1, params['RESOLUTION'])
        ax.set_xticks(major_ticks)
        ax.set_yticks(major_ticks)

        # Disable major tick labels
        ax.set_xticklabels([])
        ax.set_yticklabels([])

        # Set grid based on major ticks
        ax.grid(color='grey', linestyle='-', linewidth=0.5)

        # Set minor ticks for scale
        minor_ticks = np.arange(0, params['DIM'] + 1, int(params['DIM'] / 4))
        ax.set_xticks(minor_ticks, minor=True)
        ax.set_yticks(minor_ticks, minor=True)

        # Labels for minor ticks
        minor_tick_labels = np.arange(
            0, params['DIM'] + 1, int(params['DIM'] / 4))
        ax.set_xticklabels(minor_tick_labels, minor=True, fontsize=14)
        ax.set_yticklabels(minor_tick_labels, minor=True, fontsize=14)

    # Axis Labels
    # ax.set_xlabel('X Axis')
    # ax.set_ylabel('Y Axis')
    if _3D:
        ax.set_zlabel('Z Axis')

    # Plot start point
    print('Plotting Start Point')
    with open('cache/startPoint.txt', 'r') as f:
        point = list(csv.reader(f))[0]
    plot_point(ax, point, 'green', label='Start Point')

    print('Plotting Goal Point')
    with open('cache/goalPoint.txt', 'r') as f:
        point = list(csv.reader(f))[0]
    plot_point(ax, point, colors[3], label='Goal Point')

    ax.legend()

    plot_graph(ax, num_edges)

    write_text(plt, num_edges)

    if _3D:
        ax.view_init(elev=10, azim=270)


if __name__ == '__main__':
    # Determine whether to Graph in 3D or 2D
    global colors
    colors = get_colors()

    print('Loading RRT Configuration Parameters')
    global params
    params = rrt_config()

    global _3D
    with open(DIR_PATH + '/cache/startPoint.txt', 'r') as f:
        point = list(csv.reader(f))[0]
    _3D = False if (len(point) == 2) else True

    k = int(sys.argv[1]) if len(sys.argv) == 2 else params['NUM_CONFIGS']
    plot(k)

    # Save figure
    if len(sys.argv) != 2:
        plotname = 'graph/RRTGraph.png'
    else:
        plotname = 'graph/' + sys.argv[1] + '.png'
    plt.savefig(plotname)

    # for k in range(params['NUM_CONFIGS']):
    #     plot(k)

    #     # Save figure
    #     plotname = 'graph/' + str(k) + '.png'
    #     plt.savefig(plotname)
