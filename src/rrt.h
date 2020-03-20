#ifndef RRT
#define RRT

// Include correct library for desired dimensionality
#if _3D
#include "3D.h"
#else
#include "2D.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

#define START_CACHE "cache/startNode.txt"
#define GOAL_CACHE "cache/goalNode.txt"
#define PATH_CACHE "cache/path.txt"
#define SUCCESS_CACHE "cache/success.txt"

////////////////////////////////////////////////////////////////////////////////
// Type Definitions
////////////////////////////////////////////////////////////////////////////////

// RRT Graph
typedef struct graph {
    point_t nodes[NUMBUCKETS][NUM_NODES];
    edge_t edges[NUM_NODES];
    int existingNodes[NUMBUCKETS];

} graph_t;

////////////////////////////////////////////////////////////////////////////////
// Function Definitions
////////////////////////////////////////////////////////////////////////////////

// Initialize RRT Graph. Init edges to new point to stop valgrind issues. 
void initGraph(graph_t* graph) {
    for (int i=0; i<NUMBUCKETS; i++) graph->existingNodes[i] = 0;
    for (int i=0; i<NUM_NODES; i++) {
        // All these edges get overridden
        graph->edges[i] = (edge_t) {
            .p1 = (point_t) newPoint(),
            .p2 = (point_t) newPoint()};
    }
}

// Hash based on x value, for determining bucket
int hash(point_t p) {
    return ((int) p.x) % NUMBUCKETS;
}

// Add node to bucket in graph
void add_node_to_graph(graph_t *graph, point_t node) {
    int bucket = hash(node);
    graph->nodes[bucket][graph->existingNodes[bucket]] = node;
    graph->existingNodes[bucket]++;
}

point_t get_node_from_graph(graph_t *graph, int bucket, int i) {
    return graph->nodes[bucket][i];
}

#endif