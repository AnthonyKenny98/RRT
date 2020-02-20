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
    return ((int) p.x) % NUMBUCKETS;
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