#ifndef RRT
#define RRT

#include "object.h"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

#define START_CACHE "cache/startPoint.txt"
#define GOAL_CACHE "cache/goalPoint.txt"
#define PATH_CACHE "cache/path.txt"
#define SUCCESS_CACHE "cache/success.txt"

////////////////////////////////////////////////////////////////////////////////
// Type Definitions
////////////////////////////////////////////////////////////////////////////////

// RRT Graph
typedef struct graph {
    config_t configs[NUM_CONFIGS];
    edge_t edges[NUM_CONFIGS];
    int existingConfigs;
} graph_t;

////////////////////////////////////////////////////////////////////////////////
// Function Definitions
////////////////////////////////////////////////////////////////////////////////

// Initialize RRT Graph. Init edges to new point to stop valgrind issues. 
void initGraph(graph_t* graph) {
    graph->existingConfigs = 0;
    for (int i=0; i<NUM_CONFIGS; i++) {
        // All these edges get overridden
        graph->edges[i] = (edge_t) {
            .p1 = (point_t) newPoint(),
            .p2 = (point_t) newPoint()};
    }
}

// Add point to graph
void add_config_to_graph(graph_t *graph, config_t config) {
    graph->configs[graph->existingConfigs] = config;
    graph->existingConfigs++;
}

#endif