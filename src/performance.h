#include <time.h>
#include <stdio.h>

// Number of times RRT is run
#define NUM_EXPERIMENTS 100

// Define number of performance counters
#define NUM_CLKS 7

// Define Counter Names
#define CLK_BENCH 0
#define CLK_RRT    1
#define CLK_RRT_getRandomConfig 2
#define CLK_RRT_findNearestConfig 3
#define CLK_RRT_stepFromTo 4
#define CLK_RRT_configCollision 5
#define CLK_RRT_edgeCollision 6


// Counter for each metric
typedef struct counter {
    clock_t sum;
    clock_t temp;
    int runs;
} counter_t;

// Performance Struct for holding counters
typedef struct performance {
    counter_t counters[NUM_CLKS];
} performance_t;


// Benchmark function. 10^8 add iterations
#define BENCHMARK 100000000
void benchmark() {
    int i = 0; while (i<BENCHMARK) i++;
}

// Print performance
void print_performance(performance_t* p) {
    
    char* clk_names[NUM_CLKS] = {
        "benchmark",
        "rrt",
        "getRandomConfig",
        "findNearestConfig",
        "stepFromTo",
        "configCollisions",
        "edgeCollisions",
    };
    
    FILE *f = fopen("cache/performance.csv", "w");
    fprintf(f, "Function Stack,CPU Time:Self,time (us),time (%% of RRT),time (%% of benchmark),runs\n");
    for (int i=0; i<NUM_CLKS; i++) {
        fprintf(f,
            "%s,%f,%ld,%f,%f,%d\n",
            clk_names[i],
            (float) p->counters[i].sum / 1000000,
            p->counters[i].sum,
            (float) p->counters[i].sum / p->counters[CLK_RRT].sum * 100,
            (float) p->counters[i].sum / p->counters[CLK_BENCH].sum * 100,
            p->counters[i].runs);
    }
    fclose(f);
}

// Timing Functions
clock_t clk_micro() {
    return clock() / (CLOCKS_PER_SEC / 1000000); // Microseconds
}

void start_clk(performance_t* perf, int counter) {
    perf->counters[counter].temp = clk_micro();
    perf->counters[counter].runs++;
}

void end_clk(performance_t* perf, int counter) {
    perf->counters[counter].sum += (clk_micro() - perf->counters[counter].temp);
}