// Include correct library for desired dimensionality
#if _3D
    #include "3D.h"
    #if _OBJECT
        #define NUM_CONFIG_VERTICES 8
    #else
        #define NUM_CONFIG_VERTICES 1
    #endif
#else
    #include "2D.h"
    #if _OBJECT
        #define NUM_CONFIG_VERTICES 4
    #else
        #define NUM_CONFIG_VERTICES 1
    #endif
#endif

// Defines the size of the object. 
// Cannot be larger than resolution (one grid) of graph
#define OBJECT_SIZE 1
#define ORIGIN 0

////////////////////////////////////////////////////////////////////////////////
// Type Definitions
////////////////////////////////////////////////////////////////////////////////

// Configuration of an object
typedef struct config {
    point_t point[NUM_CONFIG_VERTICES];
} config_t;

////////////////////////////////////////////////////////////////////////////////
// Function Definitions
////////////////////////////////////////////////////////////////////////////////

// Distance between two configurations
// Using distance between origin points for simplicity
float config_distance_squared(config_t c1, config_t c2) {
    return distance_squared(c1.point[ORIGIN], c2.point[ORIGIN]);
}

// Return New Configuration
#if _3D
config_t getNewConfig(point_t originPoint) {
    config_t newConfig;
    for (int i = 0; i<NUM_CONFIG_VERTICES; i++) { 
        newConfig.point[i] = (point_t) {
            .x = originPoint.x + deltaPoints[i].x*OBJECT_SIZE,
            .y = originPoint.y + deltaPoints[i].y*OBJECT_SIZE,
            .z = originPoint.z + deltaPoints[i].z*OBJECT_SIZE
        };
    }
    return newConfig;
}
#else
config_t getNewConfig(point_t originPoint) {
    config_t newConfig;
    for (int i = 0; i<NUM_CONFIG_VERTICES; i++) { 
        newConfig.point[i] = (point_t) {
            .x = originPoint.x + deltaPoints[i].x*OBJECT_SIZE,
            .y = originPoint.y + deltaPoints[i].y*OBJECT_SIZE
        };
    }
    return newConfig;
}
#endif

// Return a new configuration with a random point as its origin point
config_t getRandomConfig() {
    return getNewConfig(getRandomPoint());
}

// Test if any of a configuration's vertices collide with an object.
// Currently only works if object is smaller than 1 grid
bool configCollision(config_t config, space_t *space) {
    for (int i=0; i<NUM_CONFIG_VERTICES; i++) {
        if (pointCollision(config.point[i], space)) {
            return true;
        }
    }
    return false;
}

edge_t drawEdge(config_t c1, config_t c2) {
    return (edge_t) {.p1 = c1.point[ORIGIN], .p2 = c2. point[ORIGIN]};
}

// Return an edge connected two configurations' origin points
bool connectConfigs(config_t c1, config_t c2, space_t* space) {
    edge_t edge;
    for (int i=0; i<NUM_CONFIG_VERTICES; i++) {
        edge = (edge_t) {.p1 = c1.point[i], .p2 = c2.point[i]};
        if (edgeCollision(edge, space)) return true;
    }
    return false;
}

// Return configuration found EPSILON away from c1 in the directon of c2
// Based of origin point for simplicity
config_t stepTowardsConfig(config_t c1, config_t c2) {
    return getNewConfig(stepTowardsPoint(c1.point[ORIGIN], c2.point[ORIGIN]));
}
