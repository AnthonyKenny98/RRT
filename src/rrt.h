#ifndef RRT
#define RRT

#define STEP_DELAY 2
#include "ogm.h"

typedef struct graph {
    point_t nodes[NUM_NODES];
    edge_t edges[NUM_NODES];
    int existingNodes;
} graph_t;

#endif