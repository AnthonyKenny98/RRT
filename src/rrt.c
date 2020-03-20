/*
* @Author: AnthonyKenny98
* @Date:   2019-10-31 11:57:52
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-03-20 11:46:03
*/

#include "rrt.h"
#include "performance.h"
// #include <unistd.h> Dont know why this is here...

point_t findNearestNode(point_t newPoint, graph_t *graph, point_t startNode) {
    point_t nearestNode = startNode;
    int bucket = hash(newPoint);
    int delta = 0;
    int count = 0;
    do {
        for (int i=0; i<graph->existingNodes[bucket+delta]; i++) {
            if (distance_squared(get_node_from_graph(graph, bucket+delta, i), newPoint) < distance_squared(nearestNode, newPoint)) {
                nearestNode = get_node_from_graph(graph, bucket+delta, i);
            }
        }
    } while (newPoint.x == nearestNode.x && newPoint.y == nearestNode.y && count < NUMBUCKETS);
    return nearestNode;
}


// Steps from point 1 to point 2 or new point
point_t stepFromTo(point_t p1, point_t p2, point_t goalNode) {
    // Epsilon * Epsilon since distance_squared
    if (distance_squared(p1, p2) < (EPSILON * EPSILON)) {
        return p2;
    }
    else {
        if (randomfloat(100.) < GOAL_BIAS) {
            return stepTowardsPoint(p1, goalNode);
        } else {
            return stepTowardsPoint(p1, p2);
        }
    }
}


int rrt(graph_t *graph, space_t *space, point_t startNode, point_t goalNode, performance_t* perf) {

    // Start Point
    add_node_to_graph(graph, startNode);

    // Init points
    point_t randomNode = startNode;
    point_t nearestNode;
    point_t newNode;

    bool pc_test, ec_test;

    for (int i=1; i<NUM_NODES; i++) {
        
        // Get Random Point that is not in collision with 
        do {

            // Get Random Node
            start_clk(perf, CLK_RRT_getRandomNode);
            randomNode = getRandomNode();
            end_clk(perf, CLK_RRT_getRandomNode);

            // Find Nearest Node in Graph
            start_clk(perf, CLK_RRT_findNearestNode);
            nearestNode = findNearestNode(randomNode, graph, startNode);
            end_clk(perf, CLK_RRT_findNearestNode);

            // Extend from Nearest Node to or towards Random Node = New Node
            start_clk(perf, CLK_RRT_stepFromTo);
            newNode = stepFromTo(nearestNode, randomNode, goalNode);
            end_clk(perf, CLK_RRT_stepFromTo);

            // Check New Node for collision
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
            add_node_to_graph(graph, newNode);
            graph->edges[i] = newEdge;
        }
        else {
            i--;
        }
    }

    point_t nearestNodeToGoal = findNearestNode(goalNode, graph, startNode);
    if (distance_squared(goalNode, nearestNodeToGoal) < EPSILON * EPSILON) {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char *argv[]) {

    // Allocate and Init Performance Tracking Struct (Loop to silence Valgrind)
    performance_t* perf = malloc(sizeof(performance_t));
    for (int i=0; i<NUM_CLKS; i++) {
        perf->counters[i].temp = 0;
        perf->counters[i].sum = 0;
        perf->counters[i].runs = 0;
    }
    
    // Included to provide a benchmark for different performance analysis
    start_clk(perf, CLK_BENCH);
    benchmark();
    end_clk(perf, CLK_BENCH);
    
    ///////////////////////////////////////////////////////////////////////////
    // SETUP STAGE
    ///////////////////////////////////////////////////////////////////////////
    
    // Configure Randomness
    srand ((unsigned int) time(NULL)*10000000);

    // Init Space
    space_t *space = malloc(sizeof(space_t));
    initOGM(space);

    // Init Graph (with Values to silence Valgrind Errors)
    graph_t *graph = malloc(sizeof(graph_t));
    initGraph(graph);


    // Init Start and Goal Node
    point_t startNode, goalNode;

    ///////////////////////////////////////////////////////////////////////////
    // Run RRT
    ///////////////////////////////////////////////////////////////////////////

    // Run RRT with same Space for NUM_EXPERIMENTS
    int success = 0;
    for (int e=0; e<NUM_EXPERIMENTS; e++) {

        // New start and End Node
        do { startNode = getRandomNode(); } while (pointCollision(startNode, space));
        do { goalNode = getRandomNode(); } while (
            (pointCollision(goalNode, space)) && (distance_squared(goalNode, startNode) > XDIM/2)
        );
        
        // Run RRT
        start_clk(perf, CLK_RRT);
        success += rrt(graph, space, startNode, goalNode, perf);
        end_clk(perf, CLK_RRT); 

        // Reset Graph for next run
        if (e<NUM_EXPERIMENTS-1) {
            free(graph);
            graph = malloc(sizeof(graph_t));
            initGraph(graph);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Log Data for Later Analysis
    ///////////////////////////////////////////////////////////////////////////

    // Start node
    FILE *f = fopen(START_CACHE, "w");
    fprint_point(f, startNode);
    fclose(f);

    // Start node
    f = fopen(GOAL_CACHE, "w");
    fprint_point(f, goalNode);
    fclose(f);

    // Path
    f = fopen(PATH_CACHE, "w");
    for (int i = 0; i<NUM_NODES - 1; i++) {
        fprint_point(f, graph->edges[i].p1);
        fprintf(f, ",");
        fprint_point(f, graph->edges[i].p2);
        fprintf(f, "\n");
    }
    fclose(f);

    f = fopen(SUCCESS_CACHE, "w");
    fprintf(f, "%d", (int)((float) success/NUM_EXPERIMENTS*100));
    fclose(f);
    
    // End Performance Tracking and Print
    print_performance(perf);
    
    // Free Memory
    free(graph);
    free(space);
    free(perf);

    return 0;
}
