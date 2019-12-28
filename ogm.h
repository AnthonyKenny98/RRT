#include "naive.h"

typedef struct _space {
    obstacle_t obstacles[NUM_OBSTACLES];
    bool ogm[XDIM/RESOLUTION][YDIM/RESOLUTION];
} _space_t;
#define space_t _space_t

// Creates NUM_OBSTACLES rectangular obstacles
void _initObstacles(_space_t *space);
#define initObstacles _initObstacles

bool _point_collision(point_t node, _space_t *space);
#define point_collision _point_collision