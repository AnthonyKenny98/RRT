"""LEarning."""
# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-05 20:46:51
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-05 21:52:28

from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import numpy as np

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d', xlim=100, ylim=100, zlim=100)
ax.invert_zaxis()


def plot_rectangle(origin, ax):
    w = 10
    l = 10
    h = 10

    add = np.array([[0, 0, 0],
                    [0, l, 0],
                   [w, l, 0],
                   [w, 0, 0],
                   [0, 0, h],
                   [0, l, h],
                   [w, l, h],
                   [w, 0, h]])

    p = np.array([[10, 10, 10],
                  [10, 20, 10],
                  [20, 20, 10],
                  [20, 10, 10],
                  [10, 10, 20],
                  [10, 20, 20],
                  [20, 20, 20],
                  [20, 10, 20]
                  ])
    p = origin + add

    def draw_face(p1):
        x = p1[:, 0]
        y = p1[:, 1]
        z = p1[:, 2]

        verts = [list(zip(x, y, z))]

        collection = Poly3DCollection(verts, linewidths=1, edgecolors='black', zsort='min')
        face_color = "red"
        collection.set_facecolor(face_color)
        ax.add_collection3d(collection)

    draw_face(np.array([p[0], p[1], p[2], p[3]]))
    draw_face(np.array([p[4], p[5], p[6], p[7]]))
    draw_face(np.array([p[0], p[1], p[5], p[4]]))
    draw_face(np.array([p[2], p[3], p[7], p[6]]))
    draw_face(np.array([p[0], p[3], p[7], p[4]]))
    draw_face(np.array([p[1], p[2], p[6], p[5]]))


origin = np.array([10, 10, 10])
plot_rectangle(origin, ax)
origin = np.array([30, 30, 30])
plot_rectangle(origin, ax)
plt.show()