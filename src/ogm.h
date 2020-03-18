#include "tools.h"

typedef struct space {
    bool ogm[XDIM][YDIM][ZDIM];
} space_t;

void initObstacles(space_t *space);

bool pointCollision(point_t node, space_t *space);

int grid_lookup(float val);