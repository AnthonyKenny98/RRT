/*
* @Author: AnthonyKenny98
* @Date:   2019-12-08 08:10:44
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-03-19 07:04:28
*/

#include "ogm.h"
#include <string.h>

void initObstacles(space_t *space) {
    // Open File
    FILE *f = fopen("cache/ogm.csv", "r");
    
    // Fill out OGM based on csv
    int temp;
    for(int i=XDIM-1;i>=0;i--){
        for(int j=0;j<YDIM;j++){
            for (int k=0; k<ZDIM;k++) {
                fscanf(f, "%i;", &temp);
                // Reverse i and j because we read csv opposite way
                // to the way the grid is set up (cartesian)
                space->ogm[j][i][k] = (bool) temp;
            }
            fscanf(f,",");
        }
        fscanf(f,"\n");  
    }
}

// Returns true if point collides with obstacle
bool pointCollision(point_t node, space_t *space) {
    return space->ogm[grid_lookup(node.x)][grid_lookup(node.y)][grid_lookup(node.z)];
}

int grid_lookup(float val) {
    return (int) max(round(val - 0.5), 0);
}