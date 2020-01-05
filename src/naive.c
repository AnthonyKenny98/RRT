/*
* @Author: AnthonyKenny98
* @Date:   2019-12-07 16:36:41
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-01-05 09:17:48
*/

#include "naive.h"

// Creates NUM_OBSTACLES rectangular obstacles
void initObstacles(space_t *space) {
    
    for (int i=0; i<NUM_OBSTACLES; i++) {
        point_t p1 = getRandomNode();
        point_t p2 = {.x = p1.x, .y = p1.y+OBSTACLE_SIZE};
        point_t p3 = {.x = p1.x+OBSTACLE_SIZE, .y = p1.y+OBSTACLE_SIZE};
        point_t p4 = {.x = p1.x+OBSTACLE_SIZE, .y = p1.y};
        obstacle_t obstacle = {.v1 = p1, .v2 = p2, .v3 = p3, .v4 = p4};
        space->obstacles[i] = obstacle;
    }
}

bool pointInRectangle(point_t node, obstacle_t obs) {
    return ((perpendicularDistance(node, (edge_t) {.p1=obs.v1, .p2=obs.v2}) < sqrt(distance_squared(obs.v1, obs.v4))) && 
        (perpendicularDistance(node, (edge_t) {.p1=obs.v2, .p2=obs.v3}) < sqrt(distance_squared(obs.v2, obs.v1)))) ||
        ((perpendicularDistance(node, (edge_t) {.p1=obs.v3, .p2=obs.v4}) < sqrt(distance_squared(obs.v3, obs.v2))) &&
        (perpendicularDistance(node, (edge_t) {.p1=obs.v4, .p2=obs.v1}) < sqrt(distance_squared(obs.v4, obs.v3))));
}

// Returns true if point collides with obstacle
// Works only for Rectangular obstacles
bool point_collision(point_t node, space_t *space) {
    for (int i=0; i<NUM_OBSTACLES; i++) {
        obstacle_t obs = space->obstacles[i];
        if (pointInRectangle(node, obs)) 
            return true;
    }
    return false;
}