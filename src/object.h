// Include correct library for desired dimensionality
#if _3D
    #include "3D.h"
    #define NUM_CONFIG_VERTICES 8
#else
    #include "2D.h"
    #define NUM_CONFIG_VERTICES 4
#endif

// Defines the size of the object. 
// Cannot be larger than resolution (one grid) of graph
#define OBJECT_SIZE = 0.5

////////////////////////////////////////////////////////////////////////////////
// Type Definitions
////////////////////////////////////////////////////////////////////////////////

// Configuration of a rectangular prism drone
typedef struct config {
    // point_t point[NUM_CONFIG_VERTICES];
    point_t point;
} config_t;

////////////////////////////////////////////////////////////////////////////////
// Function Definitions
////////////////////////////////////////////////////////////////////////////////

// Return Random Configuration
config_t getRandomConfig() {
    // config_t randomConfig;
    // for (int i = 0; i<NUM_CONFIG_VERTICES; i++) { 
    //     randomConfig.point[i] = getRandomPoint();
    // }
    // return randomConfig;
    return (config_t) {.point = getRandomPoint() };
}

bool configCollision(config_t config, space_t *space) {
    // for (int i=0; i<NUM_CONFIG_VERTICES; i++) {
    //     if (pointCollision(config.point[i], space)) {
    //         return true;
    //     }
    // }
    // return false;
    return pointCollision(config.point, space);
}

config_t stepTowardsConfig(config_t c1, config_t c2) {
    return (config_t) {.point = stepTowardsPoint(c1.point, c2.point)};
}
