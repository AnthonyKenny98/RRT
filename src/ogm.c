/*
* @Author: AnthonyKenny98
* @Date:   2019-12-08 08:10:44
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-01-05 11:37:06
*/

#include "ogm.h"

// void _initObstacles(space_t *space) {
    
//     for (int i=0; i<NUM_OBSTACLES; i++) {
//         point_t p1 = getRandomNode();
//         point_t p2 = {.x = p1.x, .y = p1.y+OBSTACLE_SIZE};
//         point_t p3 = {.x = p1.x+OBSTACLE_SIZE, .y = p1.y+OBSTACLE_SIZE};
//         point_t p4 = {.x = p1.x+OBSTACLE_SIZE, .y = p1.y};
//         obstacle_t obstacle = {.v1 = p1, .v2 = p2, .v3 = p3, .v4 = p4};
//         space->obstacles[i] = obstacle;

//         // Add to OGM
//         for (int x=0; x<=OBSTACLE_SIZE/RESOLUTION; x++) {
//             for (int y=0; y<=OBSTACLE_SIZE/RESOLUTION; y++) {
//                 int xcell = p1.x/RESOLUTION + x;
//                 int ycell = p1.y/RESOLUTION + y;
//                 if (xcell*RESOLUTION < XDIM & ycell*RESOLUTION < YDIM) {
//                     space->ogm[xcell][ycell] = 1;
//                 }
//             }
//         }
//     }
// }

void _initObstacles(space_t *space) {
    // Open File
    FILE *f = fopen("ogm.csv","r");
    
    // Fill out OGM
    int temp;
    for(int i=0;i<XDIM/RESOLUTION;i++){
        for(int j=0;j<YDIM/RESOLUTION;j++){
            fscanf(f, "%i,", &temp);
            space->ogm[i][j] = (bool) temp;
        }
        fscanf(f,"\n");  
    }
}

// Returns true if point collides with obstacle
bool _point_collision(point_t node, space_t *space) {
    return space->ogm[grid_lookup(node.x)][grid_lookup(node.y)];
}

int grid_lookup(double val) {
    return (int) val/RESOLUTION;
}