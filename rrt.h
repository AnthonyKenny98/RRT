#ifndef RRT
#define RRT

#define STEP_DELAY 2

#ifdef NAIVE
    #include "naive.h"
    #define SUFFIX "naive"
#else
    #include "ogm.h"
    #define SUFFIX "ogm"
#endif

typedef struct graph {
    point_t nodes[NUM_NODES];
    edge_t edges[NUM_NODES];
    int existingNodes;
} graph_t;

#endif