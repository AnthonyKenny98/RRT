#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "params.h"

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

// Euclidean Distance between two points
double distance_squared(point_t p1, point_t p2);

// Returns a random node in the state space
point_t getRandomNode();