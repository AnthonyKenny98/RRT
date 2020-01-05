#include "tools.h"

typedef struct space {
    bool ogm[XDIM/RESOLUTION][YDIM/RESOLUTION];
} space_t;

void initObstacles(space_t *space);

bool pointCollision(point_t node, space_t *space);

int grid_lookup(double val);