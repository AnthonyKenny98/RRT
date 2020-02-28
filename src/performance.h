#include <time.h>
#include <stdio.h>

#define NUM_EXPERIMENTS 10

// Define number of performance counters
#define NUM_CLKS 7

// Define Counter Names
#define CLK_BENCH 0
#define CLK_RRT    1
#define CLK_RRT_getRandomNode 2
#define CLK_RRT_findNearestNode 3
#define CLK_RRT_stepFromTo 4
#define CLK_RRT_pointCollision 5
#define CLK_RRT_edgeCollision 6


typedef struct counter {
    clock_t sum;
    clock_t temp;
    int runs;
} counter_t;

typedef struct performance {
    counter_t counters[NUM_CLKS];
} performance_t;


#define A_BILLION 1000000000
void run_benchmark() {
    int i = 0; while (i<A_BILLION) i++;
}

void print_performance(performance_t* p) {
    
    char* clk_names[NUM_CLKS] = {
        "RRT",
        "getRandomNode",
        "findNearestNode",
        "stepFromTo",
        "pointCollision",
        "edgeCollision",
        "BENCHMARK"
    };

    // printf("RRT Ran %d times\n", NUM_EXPERIMENTS);
    
    printf("function, time (us), time (%% of RRT), time (%% of benchmark), runs\n");
    for (int i=0; i<NUM_CLKS; i++) {
        printf(
            "%s,%ld,%f,%f,%d\n",
            clk_names[i], 
            p->counters[i].sum,
            (float) p->counters[i].sum / p->counters[CLK_RRT].sum * 100,
            (float) p->counters[i].sum / p->counters[CLK_BENCH].sum * 100,
            p->counters[i].runs);
    }
}

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