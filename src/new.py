"""LEarning."""
# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-05 20:46:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-05 22:18:23

from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import numpy as np


def rrt_config():
    """Get configuration params from params.h file."""
    params = {
        'XDIM': 100,
        'YDIM': 100,
        'ZDIM': 100,
        'RESOLUTION': 5
    }

    # Read params.h
    # with open('params.h', 'r') as file:
    #     for line in file:
    #         params[line.split(' ')[1]] = int(line.split(' ')[2].strip('\n'))
    return params


def get_ogm(params):
    """."""
    ogm = np.zeros((params['XDIM'], params['YDIM'], params['ZDIM']))

    ogm[3:6, 3:6, 3:6] = 1

    return ogm


def plot_rectangle(origin, s, ax):
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
            verts, linewidths=1, edgecolors='black', zsort='min')
        face_color = "red"
        collection.set_facecolor(face_color)
        ax.add_collection3d(collection)

    draw_face(np.array([p[0], p[1], p[2], p[3]]))
    draw_face(np.array([p[4], p[5], p[6], p[7]]))
    draw_face(np.array([p[0], p[1], p[5], p[4]]))
    draw_face(np.array([p[2], p[3], p[7], p[6]]))
    draw_face(np.array([p[0], p[3], p[7], p[4]]))
    draw_face(np.array([p[1], p[2], p[6], p[5]]))


def main():
    """."""
    params = rrt_config()

    # Create Figure
    fig = plt.figure("Configuration Space")

    # Init plot in 3D with correct axis limits
    ax = fig.add_subplot(
        111, projection='3d',
        xlim=params['XDIM'], ylim=params['YDIM'], zlim=params['ZDIM'])

    # Invert every axis to give correct cartesian look
    ax.invert_xaxis()
    ax.invert_yaxis()
    ax.invert_zaxis()

    # Get Occupancy Grid Map from RRT
    ogm = get_ogm(params)

    # Draw Occupied Grids
    for i in range(int(params['XDIM'] / params['RESOLUTION'])):
        for j in range(int(params['YDIM'] / params['RESOLUTION'])):
            for k in range(int(params['ZDIM'] / params['RESOLUTION'])):
                if ogm[i][j][k]:
                    origin = [i * params['RESOLUTION'],
                              j * params['RESOLUTION'],
                              k * params['RESOLUTION']]
                    plot_rectangle(origin, params['RESOLUTION'], ax)

    # Show Plot
    plt.show()

if __name__ == "__main__":
    main()
