#ifndef RRT
#define RRT

#include "ogm.h"

typedef struct graph {
    point_t nodes[NUMBUCKETS][NUM_NODES];
    edge_t edges[NUM_NODES];
    int existingNodes[NUMBUCKETS];

} graph_t;

// Hash based on distance from center
int hash(point_t p) {
    point_t center_point = {
        .x = XDIM/2,
        .y = YDIM/2,
        .z = ZDIM/2
    };
    double d = distance_squared(p, center_point);
    return (int) max(round(d - 0.5), 0) % NUMBUCKETS;
}

void add_node_to_graph(graph_t *graph, int i, point_t node) {
    int bucket = hash(node);
    graph->nodes[bucket][graph->existingNodes[bucket]] = node;
    graph->existingNodes[bucket]++;
}

point_t get_node_from_graph(graph_t *graph, int bucket, int i) {
    return graph->nodes[bucket][i];
}

#endif