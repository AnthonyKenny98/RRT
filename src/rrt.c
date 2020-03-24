/*
* @Author: AnthonyKenny98
* @Date:   2019-10-31 11:57:52
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-03-24 14:21:07
*/

#include "rrt.h"
#include "performance.h"
// #include <unistd.h> Dont know why this is here...

config_t findNearestConfig(config_t newConfig, graph_t *graph, config_t startConfig) {
    config_t nearestConfig = startConfig;
    int bucket = hash(newConfig);
    int delta = 0;
    int count = 0;
    do {
        for (int i=0; i<graph->existingConfigs[bucket+delta]; i++) {
            if (distance_squared(get_config_from_graph(graph, bucket+delta, i).point, newConfig.point) < distance_squared(nearestConfig.point, newConfig.point)) {
                nearestConfig = get_config_from_graph(graph, bucket+delta, i);
            }
        }
    } while (newConfig.point.x == nearestConfig.point.x && newConfig.point.y == nearestConfig.point.y && count < NUMBUCKETS);
    return nearestConfig;
}


// Steps from config 1 to config 2 or new config
config_t stepFromTo(config_t c1, config_t c2, config_t goalConfig) {
    // Epsilon * Epsilon since distance_squared
    if (distance_squared(c1.point, c2.point) < (EPSILON * EPSILON)) {
        return c2;
    }
    else {
        if (randomfloat(100.) < GOAL_BIAS) {
            return stepTowardsConfig(c1, goalConfig);
        } else {
            return stepTowardsConfig(c1, c2);
        }
    }
}


int rrt(
    graph_t *graph, space_t *space,
    config_t startConfig, config_t goalConfig,
    performance_t* perf) {

    // Start Point
    add_config_to_graph(graph, startConfig);

    // Init points
    config_t randomConfig = startConfig;
    config_t nearestConfig;
    config_t newConfig;

    bool pc_test, ec_test;

    for (int i=1; i<NUM_CONFIGS; i++) {
        
        // Get Random Point that is not in collision with 
        do {

            // Get Random Point
            start_clk(perf, CLK_RRT_getRandomConfig);
            randomConfig = getRandomConfig();
            end_clk(perf, CLK_RRT_getRandomConfig);

            // Find Nearest Point in Graph
            start_clk(perf, CLK_RRT_findNearestConfig);
            nearestConfig = findNearestConfig(randomConfig, graph, startConfig);
            end_clk(perf, CLK_RRT_findNearestConfig);

            // Extend from Nearest Point to or towards Random Point = New Point
            start_clk(perf, CLK_RRT_stepFromTo);
            newConfig = stepFromTo(nearestConfig, randomConfig, goalConfig);
            end_clk(perf, CLK_RRT_stepFromTo);

            // Check New Point for collision
            start_clk(perf, CLK_RRT_configCollision);
            pc_test = configCollision(newConfig, space);
            end_clk(perf, CLK_RRT_configCollision);

        } while (pc_test);
        
        // Draw edge
        edge_t newEdge = {.p1 = nearestConfig.point, .p2 = newConfig.point};

        start_clk(perf, CLK_RRT_edgeCollision);
        ec_test = !edgeCollisions(newEdge, space);
        end_clk(perf, CLK_RRT_edgeCollision);

        if (ec_test) {
            // Update graph
            add_config_to_graph(graph, newConfig);
            graph->edges[i] = newEdge;
        }
        else {
            i--;
        }
    }

    config_t nearestConfigToGoal = findNearestConfig(goalConfig, graph, startConfig);
    if (distance_squared(goalConfig.point, nearestConfigToGoal.point) < EPSILON * EPSILON) {
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


    // Init Start and Goal Point
    config_t startConfig, goalConfig;

    ///////////////////////////////////////////////////////////////////////////
    // Run RRT
    ///////////////////////////////////////////////////////////////////////////

    // Run RRT with same Space for NUM_EXPERIMENTS
    int success = 0;
    for (int e=0; e<NUM_EXPERIMENTS; e++) {

        // New start and End Point
        do { startConfig = getRandomConfig(); } while (configCollision(startConfig, space));
        do { goalConfig = getRandomConfig(); } while (
            (configCollision(goalConfig, space)) && (distance_squared(goalConfig.point, startConfig.point) > XDIM/2)
        );
        
        // Run RRT
        start_clk(perf, CLK_RRT);
        success += rrt(graph, space, startConfig, goalConfig, perf);
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

    // Start Point
    FILE *f = fopen(START_CACHE, "w");
    fprint_point(f, startConfig.point);
    fclose(f);

    // Start Point
    f = fopen(GOAL_CACHE, "w");
    fprint_point(f, goalConfig.point);
    fclose(f);

    // Path
    f = fopen(PATH_CACHE, "w");
    for (int i = 0; i<NUM_CONFIGS - 1; i++) {
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
