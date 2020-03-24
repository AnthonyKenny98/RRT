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
    // point_t points[NUMBUCKETS][NUM_POINTS];
    // edge_t edges[NUM_POINTS];
    // int existingPoints[NUMBUCKETS];
    config_t configs[NUMBUCKETS][NUM_CONFIGS];
    edge_t edges[NUM_CONFIGS];
    int existingConfigs[NUMBUCKETS];
} graph_t;

////////////////////////////////////////////////////////////////////////////////
// Function Definitions
////////////////////////////////////////////////////////////////////////////////

// Initialize RRT Graph. Init edges to new point to stop valgrind issues. 
void initGraph(graph_t* graph) {
    for (int i=0; i<NUMBUCKETS; i++) graph->existingConfigs[i] = 0;
    for (int i=0; i<NUM_CONFIGS; i++) {
        // All these edges get overridden
        graph->edges[i] = (edge_t) {
            .p1 = (point_t) newPoint(),
            .p2 = (point_t) newPoint()};
    }
}

// Hash based on x value, for determining bucket
int hash(config_t c) {
    return ((int) c.point.x) % NUMBUCKETS;
}

// Add point to bucket in graph
void add_config_to_graph(graph_t *graph, config_t config) {
    int bucket = hash(config);
    graph->configs[bucket][graph->existingConfigs[bucket]] = config;
    graph->existingConfigs[bucket]++;
}

config_t get_config_from_graph(graph_t *graph, int bucket, int i) {
    return graph->configs[bucket][i];
}

#endif