/*
* @Author: AnthonyKenny98
* @Date:   2019-10-31 11:57:52
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-01-21 14:38:28
*/

#include "rrt.h"

// Finds Node in current Graph nearest to New Point
point_t findNearestNode(point_t newPoint, graph_t *graph) {
    point_t nearestNode = graph->nodes[0];
    for (int i=0; i<graph->existingNodes; i++) {
        if (distance_squared(graph->nodes[i], newPoint) < distance_squared(nearestNode, newPoint)) {
            nearestNode = graph->nodes[i];
        }
    }
    return nearestNode;
}


// Steps from point 1 to point 2 or new point
point_t stepFromTo(point_t p1, point_t p2) {
    // Epsilon * Epsilon since distance_squared
    if (distance_squared(p1, p2) < (EPSILON * EPSILON)) {
        return p2;
    }
    else {
        // Squared Distance between p1 and p2 xy points
        double xyPlaneDistanceSquared = distance_squared(p1, (point_t) {.x = p2.x, .y = p2.y, .z = 0});
        
        // Find Phi, the angle between the xyplane and the z axis
        double phi = atan2((p2.z - p1.z) * (p2.z - p1.z), xyPlaneDistanceSquared);
        
        // Find Theta, the angle between the x-axis and the y-axis
        double theta = atan2((p2.y-p1.y), (p2.x - p1.x));
        
        // Find epsilonPrime, the xy distance that the new point should be
        double epsilonPrime = EPSILON * cos(phi);
        
        // Init New Point
        point_t newPoint;
        newPoint.x = p1.x + epsilonPrime*cos(theta);
        newPoint.y = p1.y + epsilonPrime*sin(theta);
        newPoint.z = p1.z + epsilonPrime*tan(phi);
        return newPoint;
    }
}

bool lineIntersectsPrism(edge_t edge, point_t prism_corner) {

    double value(double va, double vb, double t) {
    return (vb - va) * t + va;
    }

    bool lineIntersectsPlane(point_t A, point_t B, point_t C) {
        double t = (C.z - A.z) / (B.z - A.z);
        double p = value(A.x, B.x, t);
        double q = value(A.y, B.y, t);
        return ((C.x <= p) && (p <= C.x + RESOLUTION) && (C.y <= q) && (q <= C.y + RESOLUTION));
    }

    bool checkTwoFaces(point_t A, point_t B, point_t C) {
        if (lineIntersectsPlane(A, B, C)) return true;
        C.z += RESOLUTION;
        if (lineIntersectsPlane(A, B, C)) return true;
        return false;
    }

    point_t A, B, C;

    // Z Plane
    A = edge.p1;
    B = edge.p2;
    C = prism_corner;
    if (checkTwoFaces(A, B, C)) return true;

    // Y Plane
    A = (point_t) {.x = edge.p1.x, .y = edge.p1.z, .z = edge.p1.y};
    B = (point_t) {.x = edge.p2.x, .y = edge.p2.z, .z = edge.p2.y};
    C = (point_t) {.x = prism_corner.x, .y = prism_corner.z, .z = prism_corner.y};
    if (checkTwoFaces(A, B, C)) return true;

    // X Plane
    A = (point_t) {.x = edge.p1.z, .y = edge.p1.y, .z = edge.p1.x};
    B = (point_t) {.x = edge.p2.z, .y = edge.p2.y, .z = edge.p2.x};
    C = (point_t) {.x = prism_corner.z, .y = prism_corner.y, .z = prism_corner.z};
    if (checkTwoFaces(A, B, C)) return true;

    return false;
}

bool edgeCollisions(edge_t edge, space_t *space) {
    point_t min, max;
    
    // Get min and max x
    if (edge.p1.x < edge.p2.x) {
        min.x = edge.p1.x;
        max.x = edge.p2.x;
    } else {
        min.x = edge.p1.x;
        max.x = edge.p2.x;
    }
    // Get min and max y
    if (edge.p1.y < edge.p2.y) {
        min.y = edge.p1.y;
        max.y = edge.p2.y;
    } else {
        min.y = edge.p2.y;
        max.y = edge.p1.y;
    }
    // Get min and max z
    if (edge.p1.z < edge.p2.z) {
        min.z = edge.p1.z;
        max.z = edge.p2.z;
    } else {
        min.z = edge.p2.z;
        max.z = edge.p1.z;
    }

    for (int i= (int) round(min.x - 0.5); i<(int) round(max.x + 0.5); i++) {
        for (int j= (int) round(min.y - 0.5); j<(int) round(max.y + 0.5); j++) {
            for (int k= (int) round(min.z - 0.5); k<(int) round(max.z + 0.5); k++) {
                printf("Edge: (%f,%f, %f),(%f, %f, %f) Checking OGM[%i][%i][%i]\n", edge.p1.x, edge.p1.y, edge.p1.z, edge.p2.x, edge.p2.y, edge.p2.z, i, j, k);
                if (space->ogm[i][j][k]) {

                    // Set up corner of grid
                    point_t v = (point_t) {.x = i*RESOLUTION, .y = j*RESOLUTION, .z = k*RESOLUTION};
                    // Check if edge intersects with grid
                    if (lineIntersectsPrism(edge, v)) return true;
                }
            }
        }
    }
    return false;
}

void rrt(graph_t *graph, space_t *space, point_t startNode) {

    // Start Point
    graph->nodes[0] = startNode;

    // Init points
    point_t randomNode = startNode;
    point_t nearestNode;
    point_t newNode;

    for (int i=1; i<NUM_NODES; i++) {
        
        // Get Random Point that is not in collision with 
        do {
            randomNode = getRandomNode();
            // Run through all points in graph, returns point nearest to randomPoint 
            nearestNode = findNearestNode(randomNode, graph);

            // Moves an incremental distance from nearestNode to (randomPoint if distance is < Epsilon) or new point
            newNode = stepFromTo(nearestNode, randomNode);

        } while (pointCollision(newNode, space));
        
        // Draw edge
        edge_t newEdge = {.p1 = nearestNode, .p2 = newNode};

        if (!edgeCollisions(newEdge, space)) {

            // Update graph
            graph->nodes[i] = newNode;
            graph->existingNodes++;
            graph->edges[i] = newEdge;
        }
        else {
            i--;
        }
    }
}

int main(int argc, char *argv[]) {

    // Configure Randomness
    srand ((unsigned int) time(NULL)*10000000);

    // Init Space
    space_t *space = malloc(sizeof(space_t));
    initObstacles(space);

    // Init Graph
    graph_t *graph = malloc(sizeof(graph_t));
    graph->existingNodes = 0;

    // Init Start Node
    point_t startNode;
    do { startNode = getRandomNode(); } while (pointCollision(startNode, space));

    // run RRT
    clock_t start, finish, total;
    start = clock() / (CLOCKS_PER_SEC / 1000);
    rrt(graph, space, startNode);
    finish = clock() / (CLOCKS_PER_SEC / 1000);
    total = finish - start;
    printf("Total Time (milliseconds): %ld\n", total);

    // Save data for python

    // Start node
    FILE *f1 = fopen("cache/startNode.txt", "w");
    fprintf(f1, "%f, %f, %f", startNode.x, startNode.y, startNode.z);
    fclose(f1);

    // Path
    FILE *f2 = fopen("cache/path.txt", "w");
    for (int i = 0; i<NUM_NODES; i++) {
        fprintf(f2, "%f, %f, %f, %f, %f, %f\n",
            graph->edges[i].p1.x, graph->edges[i].p1.y, graph->edges[i].p1.z,
            graph->edges[i].p2.x, graph->edges[i].p2.y, graph->edges[i].p2.z);
    }
    fclose(f2);
    
    // Free Memory
    // free(graph);
    free(space);

    return 0;
}
