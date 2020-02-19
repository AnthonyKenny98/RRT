/*
* @Author: AnthonyKenny98
* @Date:   2019-10-31 11:57:52
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-02-19 15:31:47
*/

#include "rrt.h"
#include "performance.h"

// Finds Node in current Graph nearest to New Point
point_t findNearestNode(point_t newPoint, graph_t *graph, point_t startNode) {
    point_t nearestNode = startNode;
    int bucket = hash(newPoint);
    for (int i=0; i<graph->existingNodes[bucket]; i++) {
        if (distance_squared(get_node_from_graph(graph, bucket, i), newPoint) < distance_squared(nearestNode, newPoint)) {
            nearestNode = get_node_from_graph(graph, bucket, i);
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

bool lineIntersectsPrism(edge_t edge, point_t prism_corner) {

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
    
    int min_x, max_x, min_y, max_y, min_z, max_z;
    
    // Get min_p and max_p x
    if (edge.p1.x < edge.p2.x) {
        min_x = grid_lookup(edge.p1.x);
        max_x = grid_lookup(edge.p2.x);
    } else {
        min_x = grid_lookup(edge.p2.x);
        max_x = grid_lookup(edge.p1.x);
    }
    // Get min_p and max_p y
    if (edge.p1.y < edge.p2.y) {
        min_y = grid_lookup(edge.p1.y);
        max_y = grid_lookup(edge.p2.y);
    } else {
        min_y = grid_lookup(edge.p2.y);
        max_y = grid_lookup(edge.p1.y);
    }
    // Get min_p and max_p z
    if (edge.p1.z < edge.p2.z) {
        min_z = grid_lookup(edge.p1.z);
        max_z = grid_lookup(edge.p2.z);
    } else {
        min_z = grid_lookup(edge.p2.z);
        max_z = grid_lookup(edge.p1.z);
    }



    for (int i=min_x; i <= max_x; i++) {
        for (int j=min_y; j<=max_y; j++) {
            for (int k=min_z; k<=max_z; k++) {
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

void rrt(graph_t *graph, space_t *space, point_t startNode, performance_t* perf) {

    // Start Point
    add_node_to_graph(graph, 0, startNode);

    // Init points
    point_t randomNode = startNode;
    point_t nearestNode;
    point_t newNode;

    bool pc_test, ec_test;

    for (int i=1; i<NUM_NODES; i++) {
        
        // Get Random Point that is not in collision with 
        do {

            start_clk(perf, CLK_RRT_getRandomNode);
            randomNode = getRandomNode();
            end_clk(perf, CLK_RRT_getRandomNode);

            // Run through all points in graph, returns point nearest to randomPoint
            start_clk(perf, CLK_RRT_findNearestNode);
            nearestNode = findNearestNode(randomNode, graph, startNode);
            end_clk(perf, CLK_RRT_findNearestNode);

            // Moves an incremental distance from nearestNode to (randomPoint if distance is < Epsilon) or new point
            start_clk(perf, CLK_RRT_stepFromTo);
            newNode = stepFromTo(nearestNode, randomNode);
            end_clk(perf, CLK_RRT_stepFromTo);

            start_clk(perf, CLK_RRT_pointCollision);
            pc_test = pointCollision(newNode, space);
            end_clk(perf, CLK_RRT_pointCollision);

        } while (pc_test);
        
        // Draw edge
        edge_t newEdge = {.p1 = nearestNode, .p2 = newNode};

        start_clk(perf, CLK_RRT_edgeCollision);
        ec_test = !edgeCollisions(newEdge, space);
        end_clk(perf, CLK_RRT_edgeCollision);

        if (ec_test) {
            // Update graph
            add_node_to_graph(graph, i, newNode);
            graph->edges[i] = newEdge;
        }
        else {
            i--;
        }
    }
}

int main(int argc, char *argv[]) {

    // Allocate and Init Performance Tracking Struct (Loop to silence Valgrind)
    performance_t* perf = malloc(sizeof(performance_t));
    for (int i=0; i<NUM_CLKS; i++) {
        perf->counters[i].temp = 0;
        perf->counters[i].sum = 0;
    }
    
    // Start Clk
    start_clk(perf, CLK_TOTAL);
    
    ///////////////////////////////////////////////////////////////////////////
    // SETUP STAGE
    ///////////////////////////////////////////////////////////////////////////
    start_clk(perf, CLK_SETUP);
    // Configure Randomness
    srand ((unsigned int) time(NULL)*10000000);

    // Init Space
    space_t *space = malloc(sizeof(space_t));
    initObstacles(space);

    // Init Graph (with Values to silence Valgrind Errors)
    graph_t *graph = malloc(sizeof(graph_t));
    for (int i=0; i<NUMBUCKETS; i++) graph->existingNodes[i] = 0;
    for (int i=0; i<NUM_NODES; i++) {
        graph->edges[i] = (edge_t) {
            .p1 = (point_t) {.x=0, .y=0, .z=0},
            .p2 = (point_t) {.x=0, .y=0, .z=0}};
    }

    // Init Start Node
    point_t startNode; // = {.x=0, .y=0, .z=0}; // Init with vals to silence valgrnd
    do { startNode = getRandomNode(); } while (pointCollision(startNode, space));


    end_clk(perf, CLK_SETUP);
    ///////////////////////////////////////////////////////////////////////////
    // Run RRT
    ///////////////////////////////////////////////////////////////////////////

    start_clk(perf, CLK_RRT);
    rrt(graph, space, startNode, perf);
    end_clk(perf, CLK_RRT);
    

    ///////////////////////////////////////////////////////////////////////////
    // Log Data for Later Analysis
    ///////////////////////////////////////////////////////////////////////////
    start_clk(perf, CLK_LOG);

    // Start node
    FILE *f1 = fopen("cache/startNode.txt", "w");
    fprintf(f1, "%f, %f, %f", startNode.x, startNode.y, startNode.z);
    fclose(f1);

    // Path
    FILE *f2 = fopen("cache/path.txt", "w");
    for (int i = 0; i<NUM_NODES - 1; i++) {
        fprintf(f2, "%f, %f, %f, %f, %f, %f\n",
            graph->edges[i].p1.x, graph->edges[i].p1.y, graph->edges[i].p1.z,
            graph->edges[i].p2.x, graph->edges[i].p2.y, graph->edges[i].p2.z);
    }
    fclose(f2);


    end_clk(perf, CLK_LOG);

    
    // End Performance Tracking and Print
    end_clk(perf, CLK_TOTAL);
    print_performance(perf);
    
    // Free Memory
    free(graph);
    free(space);
    free(perf);

    return 0;
}
