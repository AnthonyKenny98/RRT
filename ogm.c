/*
* @Author: AnthonyKenny98
* @Date:   2019-12-08 08:10:44
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2019-12-10 15:44:13
*/

#include "ogm.h"

void _initObstacles(space_t *space) {
    
    for (int i=0; i<NUM_OBSTACLES; i++) {
        point_t p1 = getRandomNode();
        point_t p2 = {.x = p1.x, .y = p1.y+OBSTACLE_SIZE};
        point_t p3 = {.x = p1.x+OBSTACLE_SIZE, .y = p1.y+OBSTACLE_SIZE};
        point_t p4 = {.x = p1.x+OBSTACLE_SIZE, .y = p1.y};
        obstacle_t obstacle = {.v1 = p1, .v2 = p2, .v3 = p3, .v4 = p4};
        space->obstacles[i] = obstacle;

        // Add to OGM
        for (int x=0; x<=OBSTACLE_SIZE/RESOLUTION; x++) {
            for (int y=0; y<=OBSTACLE_SIZE/RESOLUTION; y++) {
                int xcell = p1.x/RESOLUTION + x;
                int ycell = p1.y/RESOLUTION + y;
                if (xcell*RESOLUTION < XDIM & ycell*RESOLUTION < YDIM) {
                    space->ogm[xcell][ycell] = 1;
                }
            }
        }
    }
}

// Returns true if point collides with obstacle
bool _point_collision(point_t node, space_t *space) {
    return space->ogm[(int) node.x/RESOLUTION][(int) node.y/RESOLUTION];
}