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
    float x;
    float y;
    float z;
} point_t;

typedef struct edge {
    point_t p1 ;
    point_t p2;
} edge_t;


// Function Declarations

// Returns a Random float. Requires srand(time(NULL)) to be called in main
float randomfloat(int max);


// Euclidean Distance between two points
float distance_squared(point_t p1, point_t p2);


// Returns a random node in the state space
point_t getRandomNode();
