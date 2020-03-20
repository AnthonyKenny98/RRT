#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "params.h"


// Minimum for any type
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


// Maximum for any type
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

float greaterThan(float x1, float x2, float x0) {
    return (x0-x1)/(x2-x1);
}

float lessThan(float x1, float x2, float x0, float X) {
    return (x0-x1+X)/(x2-x1);
}

// Used by 2D and 3D for determining in which grid a point lies.
int grid_lookup(float val) {
    return (int) max(round(val - 0.5), 0);
}


// Returns a Random float. Requires srand(time(NULL)) to be called in main.
float randomfloat(int max) {
    return ((float)rand() / (float)RAND_MAX) * max;
}