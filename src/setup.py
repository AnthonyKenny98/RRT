"""Setup."""
# -*- coding: utf-8 -*-
# @Author: AnthonyKenny98
# @Date:   2020-01-05 11:05:27
# @Last Modified by:   AnthonyKenny98
# @Last Modified time: 2020-01-05 11:35:56

import csv

rrt_params = {}

with open('params.h', 'r') as file:
    for line in file:
        key, val = line.split(' ')[1], line.split(' ')[2].strip('\n')
        rrt_params[key] = int(val)

if (rrt_params['XDIM'] % rrt_params['RESOLUTION'] != 0) or \
        (rrt_params['YDIM'] % rrt_params['RESOLUTION'] != 0):
    print("WARNING: Resolution Dimension mismatch")

with open('ogm.csv', 'w') as ocm:
    writer = csv.writer(ocm, delimiter=',')
    for i in range(int(rrt_params['YDIM'] / rrt_params['RESOLUTION'])):
        writer.writerow(
            [0] * int(rrt_params['XDIM'] / rrt_params['RESOLUTION']))
