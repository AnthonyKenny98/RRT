/*
* @Author: AnthonyKenny98
* @Date:   2019-11-01 15:45:21
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-01-22 14:47:26
*/

#include "tools.h"

// Returns a Random Double
double randomDouble(int max) {
    return ((double)rand() / (double)RAND_MAX) * max;
}

// Euclidean Distance between two points
double distance_squared(point_t p1, point_t p2) {
    return ((p1.x-p2.x)*(p1.x-p2.x)) + 
           ((p1.y-p2.y)*(p1.y-p2.y)) +
           ((p1.z-p2.z)*(p1.z-p2.z));
}

// Returns a random node in the state space
point_t getRandomNode() {
    return (point_t) {.x = randomDouble(XDIM - 1),
                      .y = randomDouble(YDIM - 1),
                      .z = randomDouble(ZDIM - 1)};
}