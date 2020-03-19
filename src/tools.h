#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "params.h"

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

// Type definitions
#if _3D
typedef struct point {
    float x;
    float y;
    float z;
} point_t;
#else
typedef struct point {
    float x;
    float y;
} point_t;
#endif

typedef struct edge {
    point_t p1;
    point_t p2;
} edge_t;


// Function Declarations

// Returns a Random float. Requires srand(time(NULL)) to be called in main
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
