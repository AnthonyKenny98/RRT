"""Generally applicable plotting tool."""
# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-03-30 06:23:57
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-04-06 12:06:51

import matplotlib.pyplot as plt
import numpy as np

FIGSIZE = (15, 10)
TITLE_FONTSIZE = 30
SUBTITLE_FONTSIZE = 20
LEGEND_FONTSIZE = 16
AXIS_FONTSIZE = 24
TICK_LABEL_FONTSIZE = 18
FRAMEON = True

COLORS = {
    'grey': '#999999',
    'blue': '#4285F4',
    'green': '#34A853',
    'yellow': '#FBBC05',
    'red': '#EA4335'
}


class Figure():
    """Base Figure Class."""

    def __init__(
            self,
            title='Title',
            subtitle='Subtitle',
            xlabel='Xlabel (units)',
            ylabel='Ylabel (units)'
    ):
        """Default Scatter Plot Function."""
        self.fig = plt.figure(figsize=FIGSIZE, dpi=300, frameon=FRAMEON)
        plt.figtext(.5, .94, title, fontsize=TITLE_FONTSIZE, ha='center')
        plt.figtext(.5, .90, subtitle, fontsize=SUBTITLE_FONTSIZE, ha='center')

        # Define Subplot
        self.ax = self.fig.add_subplot(1, 1, 1)

        # Title, subtitle and axis labels
        self.ax.set_xlabel(xlabel, fontsize=AXIS_FONTSIZE)
        self.ax.set_ylabel(ylabel, fontsize=AXIS_FONTSIZE)

        self.ax.grid(color='gray', axis='y')

        self.colors = list(COLORS.values())

        self.x, self.ys = [], []

    def _get_color(self):
        if len(self.colors) == 0:
            self.colors = list(COLORS.values())
        return self.colors.pop(0)

    def add_series(self, x, y, label):
        """Add a series to the plot."""
        # if len(self.x) is not 0 and self.x != x:
        #     print("ERROR")
        #     return
        self.x = x
        self.ys.append({'data': y, 'label': label})

    def plot(self, mode='scatter'):
        """Construct Plot in a given mode."""
        if mode is 'scatter':
            for y in self.ys:
                self.ax.scatter(
                    self.x, y['data'],
                    label=y['label'], color=COLORS['red']
                )

        elif mode is 'stack':
            self.ax.stackplot(
                self.x,
                [y['data'] for y in self.ys],
                colors=COLORS.values(),
                labels=[y['label'] for y in self.ys])

    def add_legend(self):
        """Add Legend To Plot."""
        plt.legend(fontsize=LEGEND_FONTSIZE)

    def setup_axes(self):
        """Setup all Axes."""
        self.set_axis_limit()
        self.set_axis_ticks()
        self.set_axis_tick_labels()

    def set_axis_limit(self, x=None, y=None):
        """Set limits of axis."""
        # X Axis
        if x is not None:
            self.xlim = x
        else:
            self.xlim = (min(self.x), max(self.x))

        # Y axis
        if y is not None:
            self.ylim = y
        else:
            self.ylim = (
                min([min(ys['data']) for ys in self.ys]),
                max([max(ys['data']) for ys in self.ys])
            )

        self.ax.set_xlim(self.xlim)
        self.ax.set_ylim(self.ylim)

    def set_axis_ticks(self, x=None, y=None):
        """Set ticks of Axis."""
        # X Axis
        if x is not None:
            self.ax.set_xticks(x)
        else:
            self.ax.set_xticks(
                np.linspace(self.xlim[0], self.xlim[1], len(self.x)))

        # Y Axis
        if y is not None:
            self.ax.set_yticks(y)
        else:
            self.ax.set_yticks(np.linspace(self.ylim[0], self.ylim[1], 11))

    def set_axis_tick_labels(self, x=None, y=None):
        """Add axis labels to plot."""
        if x is not None:
            self.ax.set_xticklabels(x, fontsize=TICK_LABEL_FONTSIZE)
        if y is not None:
            self.ax.set_yticklabels(y, fontsize=TICK_LABEL_FONTSIZE)

    def save(self, path):
        """Save Figure."""
        plt.savefig(path)
