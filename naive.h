#include "tools.h"

typedef struct obstacle {
    point_t v1;
    point_t v2;
    point_t v3;
    point_t v4;
} obstacle_t;

typedef struct space {
    obstacle_t obstacles[NUM_OBSTACLES];
} space_t;

// Creates NUM_OBSTACLES rectangular obstacles
void initObstacles(space_t *space);

bool pointInRectangle(point_t node, obstacle_t obs);

// Returns true if point collides with obstacle
// Works only for Rectangular obstacles
bool point_collision(point_t node, space_t *space);