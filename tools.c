/*
* @Author: AnthonyKenny98
* @Date:   2019-11-01 15:45:21
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2019-12-08 09:10:02
*/

#include "tools.h"

// Delay in milliseconds
void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

// Returns a Random Double
double randomDouble(int max) {
    return ((double)rand() / (double)RAND_MAX) * max;
}

// Euclidean Distance between two points
double distance(point_t p1, point_t p2) {
    return sqrt(((p1.x-p2.x)*(p1.x-p2.x)) + ((p1.y-p2.y)*(p1.y-p2.y)));
    // return sqrt(pow((p1.x-p2.x),2) + pow((p1.y-p2.y),2));
}

// Returns a random node in the state space
point_t getRandomNode() {
    return (point_t) {.x = randomDouble(XDIM), .y = randomDouble(YDIM)};
}

double perpendicularDistance(point_t node, edge_t line) {
    double m = (line.p1.y - line.p2.y) / (line.p1.x - line.p2.x);
    double a = (line.p1.y - line.p2.y);
    double b = (line.p2.x - line.p1.x);
    double c = (line.p1.x - line.p2.x)*line.p1.y + (line.p2.y - line.p1.y)*line.p1.x;
    return (double)(fabs(a * node.x + b * node.y + c)) / (sqrt(pow(a,2) + pow(b,2)));
}