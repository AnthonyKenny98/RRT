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
} point_t;

typedef struct edge {
    point_t p1 ;
    point_t p2;
} edge_t;


// Function Declarations

// Delay in milliseconds
void delay(int milliseconds);

// Returns a Random Double. Requires srand(time(NULL)) to be called in main
double randomDouble(int max);

// Euclidean Distance between two points
double distance(point_t p1, point_t p2);

// Returns a random node in the state space
point_t getRandomNode();

double perpendicularDistance(point_t node, edge_t line);