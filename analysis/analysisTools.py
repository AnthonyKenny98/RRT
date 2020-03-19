"""Provide helper functions required for analysis."""

# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-03-19 10:19:05
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-03-19 10:19:57


import subprocess


def call(command):
    """Wrap subprocess call for bash use."""
    subprocess.call(command, shell=True, executable='/bin/bash')
