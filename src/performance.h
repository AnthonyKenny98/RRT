#include <time.h>
#include <stdio.h>

// Define number of performance counters
#define NUM_CLKS 9

// Define Counter Names
#define CLK_TOTAL  0
#define CLK_SETUP  1
#define CLK_RRT    2
#define CLK_RRT_getRandomNode 3
#define CLK_RRT_findNearestNode 4
#define CLK_RRT_stepFromTo 5
#define CLK_RRT_pointCollision 6
#define CLK_RRT_edgeCollision 7
#define CLK_LOG 8

typedef struct counter {
    clock_t sum;
    clock_t temp;
} counter_t;

typedef struct performance {
    counter_t counters[NUM_CLKS];
} performance_t;

void print_performance(performance_t* performance) {
    printf("Overall Total = %ld\n", performance->counters[CLK_TOTAL].sum);
    printf("    Setup = %ld\n", performance->counters[CLK_SETUP].sum);
    printf("    RRT   = %ld\n", performance->counters[CLK_RRT].sum);
    printf("        getRandomNode = %ld\n", performance->counters[CLK_RRT_getRandomNode].sum);
    printf("        findNearestNode = %ld\n", performance->counters[CLK_RRT_findNearestNode].sum);
    printf("        stepFromTo = %ld\n", performance->counters[CLK_RRT_stepFromTo].sum);
    printf("        pointCollision = %ld\n", performance->counters[CLK_RRT_pointCollision].sum);
    printf("        edgeCollision = %ld\n", performance->counters[CLK_RRT_edgeCollision].sum);
    printf("    Log   = %ld\n", performance->counters[CLK_LOG].sum);
}

clock_t clk_milli() {
    // return clock() / (CLOCKS_PER_SEC / 1000); // Milliseconds
    return clock() / (CLOCKS_PER_SEC / 1000000); // Microseconds
}

void start_clk(performance_t* perf, int counter) {
    perf->counters[counter].temp = clk_milli();
}

void end_clk(performance_t* perf, int counter) {
    perf->counters[counter].sum += clk_milli() - perf->counters[counter].temp;
}