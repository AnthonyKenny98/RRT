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

typedef struct point {
    double x;
    double y;
    double z;
} point_t;

typedef struct edge {
    point_t p1 ;
    point_t p2;
} edge_t;


// Function Declarations

// Returns a Random Double. Requires srand(time(NULL)) to be called in main
double randomDouble(int max);
// {
//     return ((double)rand() / (double)RAND_MAX) * max;
// }

// Euclidean Distance between two points
double distance_squared(point_t p1, point_t p2);
 // {
 //    return ((p1.x-p2.x)*(p1.x-p2.x)) + 
 //           ((p1.y-p2.y)*(p1.y-p2.y)) +
 //           ((p1.z-p2.z)*(p1.z-p2.z));
// }

// Returns a random node in the state space
point_t getRandomNode();
//  {
//     return (point_t) {.x = randomDouble(XDIM),
//                       .y = randomDouble(YDIM),
//                       .z = randomDouble(ZDIM)};
// }