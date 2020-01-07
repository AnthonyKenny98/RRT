/*
* @Author: AnthonyKenny98
* @Date:   2019-10-31 11:57:52
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-01-07 15:29:28
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
        double theta = atan2((p2.y-p1.y), (p2.x - p1.x));
        point_t newPoint;
        newPoint.x = p1.x + EPSILON*cos(theta);
        newPoint.y = p1.y + EPSILON*sin(theta);
        return newPoint;
    }
}

bool LineIntersectsLine(edge_t e1, edge_t e2) {
    double q = (e1.p1.y - e2.p1.y) * (e2.p2.x - e2.p1.x) - (e1.p1.x - e2.p1.x) * (e2.p2.y - e2.p1.y);
    double d = (e1.p2.x - e1.p1.x) * (e2.p2.y - e2.p1.y) - (e1.p2.y - e1.p1.y) * (e2.p2.x - e2.p1.x);

    if( d == 0 )
    {
        return false;
    }

    double r = q / d;

    q = (e1.p1.y - e2.p1.y) * (e1.p2.x - e1.p1.x) - (e1.p1.x - e2.p1.x) * (e1.p2.y - e1.p1.y);
    double s = q / d;

    if( r < 0 || r > 1 || s < 0 || s > 1 )
    {
        return false;
    }

    return true;
}

bool edgeCollisions(edge_t edge, space_t *space) {
    for (int i=0; i<XDIM/RESOLUTION; i++) {
        for (int j=0; j<YDIM/RESOLUTION; j++) {
            if (space->ogm[i][j]) {

                // Set up edges of grid
                point_t v1 = (point_t) {.x = i*RESOLUTION, .y = j*RESOLUTION};
                point_t v2 = (point_t) {.x = i*RESOLUTION, .y = j*RESOLUTION + RESOLUTION};
                point_t v3 = (point_t) {.x = i*RESOLUTION + RESOLUTION, .y = j*RESOLUTION + RESOLUTION};
                point_t v4 = (point_t) {.x = i*RESOLUTION + RESOLUTION, .y = j*RESOLUTION};
                
                if (LineIntersectsLine(edge, (edge_t) {.p1 = v1, .p2 = v2}) ||
                    LineIntersectsLine(edge, (edge_t) {.p1 = v2, .p2 = v3}) ||
                    LineIntersectsLine(edge, (edge_t) {.p1 = v3, .p2 = v4}) ||
                    LineIntersectsLine(edge, (edge_t) {.p1 = v4, .p2 = v1})) {
                    return true;
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

    // Init Start and End Nodes
    do { startNode = getRandomNode(); } while (pointCollision(startNode, space));

    // run RRT
    // clock_t start, finish, total;
    // start = clock() / (CLOCKS_PER_SEC / 1000);
    // rrt(graph, space, startNode);
    // finish = clock() / (CLOCKS_PER_SEC / 1000);
    // total = finish - start;
    // printf("Total Time (milliseconds): %ld\n", total);

    // Save data for python
    FILE *f = fopen("cache/startNode.txt", "w");
    fprintf(f, "%f, %f, %f", startNode.x, startNode.y, startNode.z);
    fclose(f);

    // GUI
    // if (argc > 1 && !strcmp(argv[1], "-gui")) {
    //     FILE *pipe = popen("gnuplot -persist", "w");
    //     FILE *temp = fopen("path.temp", "w");
    //     FILE *start = fopen("start.temp", "w");

    //     // Set axis ranges
    //     fprintf(pipe,"set size square 1,1\n");
    //     fprintf(pipe,"set key outside\n");
    //     fprintf(pipe,"set xrange [0:%d]\n", XDIM);
    //     fprintf(pipe,"set yrange [0:%d]\n", YDIM);


    //     // Set Start and End Points
    //     fprintf(start, "%lf %lf \n", startNode.x, startNode.y);

    //     // Set Obstacles
    //     int object = 1;
    //     for (int i=0; i<XDIM/RESOLUTION; i++) {
    //         for (int j=0; j<YDIM/RESOLUTION; j++) {
    //             if (space->ogm[i][j]) {
    //                 fprintf(pipe, 
    //                         "set object %d rect from %d,%d to %d,%d fc lt 3 back\n", 
    //                         object, 
    //                         i*RESOLUTION, 
    //                         j*RESOLUTION, 
    //                         i*RESOLUTION+RESOLUTION,
    //                         j*RESOLUTION+RESOLUTION);
    //                 object++;
    //             }
    //         }
    //     }

    //     // Plot path
    //     char *gnu_command;
    //     fprintf(pipe,"set title \"Number of Nodes: %d\"\n", NUM_NODES);
    //     for (int a=1; a < graph->existingNodes+1; a++)
    //     {
    //         fprintf(temp, "%lf %lf %lf %lf\n", 
    //             graph->edges[a].p1.x, graph->edges[a].p1.y, 
    //             graph->edges[a].p2.x, graph->edges[a].p2.y);
    //     }
    //     gnu_command = "plot \
    //                         'path.temp' using 1:2:($3-$1):($4-$2) with vectors nohead lw 0.5 lc rgb \"red\" front title \'RRT\', \
    //                         'start.temp' with point pointtype 3 ps 2 lc rgb \"blue\" title \'Start Node\'\n";
    //     fprintf(pipe, "%s\n", gnu_command);

    //     //  Close Files
    //     fclose(start);
    //     fclose(temp);
    //     fclose(pipe); 

    //     // Delete Temporary Files
    //     remove("path.temp");
    //     remove("start.temp");  
    // }
    
    // Free Memory
    // free(graph);
    free(space);

    return 0;
}
