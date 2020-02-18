#include <time.h>
#include <stdio.h>

typedef struct counter {
    clock_t sum;
    clock_t temp;
} counter_t;

typedef struct performance {
    // Total
    counter_t total;
    
    // Setup
    counter_t setup;
    counter_t setup_space;
    counter_t setup_graph;
    counter_t setup_start;
    
    // RRT
    counter_t rrt;
    counter_t rrt_getRandomNode;
    counter_t rrt_findNearestNode;
    counter_t rrt_stepFromTo;
    counter_t rrt_pointCollision;
    counter_t rrt_edgeCollision;
    counter_t log;
} performance_t;

void print_performance(performance_t performance) {
    printf("Overall Total = %ld\n", performance.total.sum);
    printf("    Setup = %ld\n", performance.setup.sum);
    printf("        Space = %ld\n", performance.setup_space.sum);
    printf("        Graph = %ld\n", performance.setup_graph.sum);
    printf("        Start = %ld\n", performance.setup_start.sum);
    printf("    RRT   = %ld\n", performance.rrt.sum);
    printf("        getRandomNode = %ld\n", performance.rrt_getRandomNode.sum);
    printf("        findNearestNode = %ld\n", performance.rrt_findNearestNode.sum);
    printf("        stepFromTo = %ld\n", performance.rrt_stepFromTo.sum);
    printf("        pointCollision = %ld\n", performance.rrt_pointCollision.sum);
    printf("        edgeCollision = %ld\n", performance.rrt_edgeCollision.sum);
    printf("    Log   = %ld\n", performance.log.sum);
    return;
}

clock_t clk_milli() {
    // return clock() / (CLOCKS_PER_SEC / 1000); // Milliseconds
    return clock() / (CLOCKS_PER_SEC / 1000000); // Microseconds
}

counter_t start_clk(counter_t counter) {
    counter.temp = clk_milli();
    return counter;
}

counter_t end_clk(counter_t counter) {
    counter.sum += clk_milli() - counter.temp;
    return counter;
}