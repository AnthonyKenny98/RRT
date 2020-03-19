/*
* @Author: AnthonyKenny98
* @Date:   2019-11-01 15:45:21
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-03-20 10:41:29
*/

#include "tools.h"

// Returns a Random float
float randomfloat(int max) {
    return ((float)rand() / (float)RAND_MAX) * max;
}

// Euclidean Distance between two points
float distance_squared(point_t p1, point_t p2) {
    return ((p1.x-p2.x)*(p1.x-p2.x)) + 
           ((p1.y-p2.y)*(p1.y-p2.y)) +
           ((p1.z-p2.z)*(p1.z-p2.z));
}

// Returns a random node in the state space
point_t getRandomNode() {
    return (point_t) {.x = randomfloat(XDIM - 1),
                      .y = randomfloat(YDIM - 1),
                      .z = randomfloat(ZDIM - 1)};
}

point_t newPoint() {
    return (point_t) {.x = 0., .y=0., .z=0.};
}