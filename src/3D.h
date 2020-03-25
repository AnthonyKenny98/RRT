#include "tools.h"

#define SEGMENTS EPSILON

////////////////////////////////////////////////////////////////////////////////
// Type Definitions
////////////////////////////////////////////////////////////////////////////////

// Point 
typedef struct point {
    float x;
    float y;
    float z;
} point_t;

// Edge
typedef struct edge {
    point_t p1;
    point_t p2;
} edge_t;

// Define Space as Occupancy Grid Map
typedef struct space {
    bool ogm[XDIM][YDIM][ZDIM];
} space_t;

// For defining an object
point_t deltaPoints[8] = {
    (point_t) {.x=0, .y=0, .z=0},
    (point_t) {.x=1, .y=0, .z=0},
    (point_t) {.x=0, .y=1, .z=0},
    (point_t) {.x=1, .y=1, .z=0},
    (point_t) {.x=0, .y=0, .z=1},
    (point_t) {.x=1, .y=0, .z=1},
    (point_t) {.x=0, .y=1, .z=1},
    (point_t) {.x=1, .y=1, .z=1}
};


////////////////////////////////////////////////////////////////////////////////
// Function Definitions
////////////////////////////////////////////////////////////////////////////////

// Euclidean Distance between two points
float distance_squared(point_t p1, point_t p2) {
    return ((p1.x-p2.x)*(p1.x-p2.x)) + 
           ((p1.y-p2.y)*(p1.y-p2.y)) +
           ((p1.z-p2.z)*(p1.z-p2.z));
}

// Return Random Point
point_t getRandomPoint() {
    return (point_t) {.x = randomfloat(XDIM - 1),
                      .y = randomfloat(YDIM - 1),
                      .z = randomfloat(ZDIM - 1)};
}

// Return new point at origin
point_t newPoint() {return (point_t) {.x = 0., .y=0., .z=0.};}


// Initialize OGM from CSV
void initOGM(space_t *space) {
    // Open File
    FILE *f = fopen("cache/ogm.csv", "r");
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

// Given two points, step by distance EPSILON from p1 towards p2
point_t stepTowardsPoint(point_t p1, point_t p2) {
    // xyzDistanceSquared
    float xyzDistance = sqrt(distance_squared(p1, p2));

    // Squared Distance between p1 and p2 xy points (use p1.z to keep in same xy plane)
    float xyDistance = sqrt(distance_squared(p1, (point_t) {.x = p2.x, .y = p2.y, .z = p1.z}));
    
    // Find Phi, the angle between the xyplane and the z axis
    // float phi = atan2((p2.z - p1.z) * (p2.z - p1.z), xyDistanceSquared);
    float phi = atan2((p2.z - p1.z), xyDistance);
    
    // Find Theta, the angle between the x-axis and the y-axis
    float theta = atan2((p2.y-p1.y), (p2.x - p1.x));
    
    // Find xyEpsilon, the xy distance that the new point should be
    float xyEpsilon = EPSILON * cos(phi);
    
    // Init New Point
    point_t newPoint;
    newPoint.x = p1.x + xyEpsilon*cos(theta);
    newPoint.y = p1.y + xyEpsilon*sin(theta);
    newPoint.z = p1.z + EPSILON*sin(phi);
    return newPoint;
}

bool pointCollision(point_t point, space_t *space) {
    return space->ogm[grid_lookup(point.x)][grid_lookup(point.y)][grid_lookup(point.z)];
}

// Return midpoint of a given edge
float splitDist(float p1, float p2) {
    return (p2 - p1) / SEGMENTS;
}

// Return true if point lies within grid, else false
bool pointInGrid(point_t point, point_t grid) {
    return (
        (grid.x <= point.x && point.x <= grid.x + RESOLUTION) &&
        (grid.y <= point.y && point.y <= grid.y + RESOLUTION) &&
        (grid.z <= point.z && point.z <= grid.z + RESOLUTION)
    );
}

// Return true if edge intersects given grid, else false
bool segmentIntersectsGrid(edge_t edge, point_t grid) {
    float x_delta = splitDist(edge.p1.x, edge.p2.x);
    float y_delta = splitDist(edge.p1.y, edge.p2.y);
    float z_delta = splitDist(edge.p1.z, edge.p2.z);
    
    bool collision = false;
    for (int i=0; i<=SEGMENTS; i++) {
        if (pointInGrid(
            (point_t) {
                .x = edge.p1.x + x_delta * i,
                .y = edge.p1.y + y_delta * i,
                .z = edge.p1.z + z_delta * i,
            }, grid)) collision = true;
    }
    return collision;
}

bool edgeCollision(edge_t edge, space_t *space) {
    
    int min_x, max_x, min_y, max_y, min_z, max_z;
    
    // Get min_p and max_p x
    if (edge.p1.x < edge.p2.x) {
        min_x = grid_lookup(edge.p1.x);
    } else {
        min_x = grid_lookup(edge.p2.x);
    }
    // Get min_p and max_p y
    if (edge.p1.y < edge.p2.y) {
        min_y = grid_lookup(edge.p1.y);
    } else {
        min_y = grid_lookup(edge.p2.y);
    }
    // Get min_p and max_p z
    if (edge.p1.z < edge.p2.z) {
        min_z = grid_lookup(edge.p1.z);
    } else {
        min_z = grid_lookup(edge.p2.z);
    }

    if (XDIM - EPSILON < min_x) {
        min_x = XDIM-EPSILON;
    }
    if (YDIM - EPSILON < min_y) {
        min_y = YDIM-EPSILON;
    }
    if (ZDIM - EPSILON < min_z) {
        min_z = ZDIM-EPSILON;
    }

    max_x=min_x+EPSILON;
    max_y=min_y+EPSILON;
    max_z=min_z+EPSILON;

    // min_x = 0;
    // min_y=0;
    // min_z=0;
    // max_x=XDIM;
    // max_y=YDIM;
    // max_z=ZDIM;

    for (int i=min_x; i <= max_x; i++) {
        for (int j=min_y; j<=max_y; j++) {
            for (int k=min_z; k<=max_z; k++) {
                if (space->ogm[i][j][k]) {

                    // Set up corner of grid
                    point_t v = (point_t) {.x = i, .y = j, .z = k};
                    // Check if edge intersects with grid
                    if (segmentIntersectsGrid(edge, v)) return true;
                }
            }
        }
    }
    return false;
}

void fprint_point(FILE* f, point_t p) {
    fprintf(f, "%f, %f, %f", p.x, p.y, p.z);
}