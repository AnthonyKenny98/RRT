#include "tools.h"

////////////////////////////////////////////////////////////////////////////////
// Space Type Definitions
////////////////////////////////////////////////////////////////////////////////

// Point
typedef struct point {
    float x;
    float y;
} point_t;

// Edge
typedef struct edge {
    point_t p1;
    point_t p2;
} edge_t;

// Define Space as Occupancy Grid Map
typedef struct space {
    bool ogm[XDIM][YDIM];
} space_t;

// For defining an object
point_t deltaPoints[4] = {
    (point_t) {.x=0, .y=0},
    (point_t) {.x=1, .y=0},
    (point_t) {.x=0, .y=1},
    (point_t) {.x=1, .y=1}
};
////////////////////////////////////////////////////////////////////////////////
// Space Function Definitions
////////////////////////////////////////////////////////////////////////////////

// Euclidean Distance between two points
float distance_squared(point_t p1, point_t p2) {
    return ((p1.x-p2.x)*(p1.x-p2.x)) + 
           ((p1.y-p2.y)*(p1.y-p2.y));
}

// Return Random Point
point_t getRandomPoint() {
    return (point_t) {.x = randomfloat(XDIM - 1),
                      .y = randomfloat(YDIM - 1)};
}

// Return new point at origin
point_t newPoint() {return (point_t) {.x = 0., .y=0.};}

// Initialize OGM from CSV
// Takes the same templates as 3D but only cares about z=0
void initOGM(space_t *space) {
    // Open File
    FILE *f = fopen("cache/ogm.csv","r");
    int temp;
    for(int i=XDIM-1;i>=0;i--){
        for(int j=0;j<YDIM;j++){
            for(int k=0; k<ZDIM; k++) {
                fscanf(f, "%i;", &temp);
                if (k==0) {
                    space->ogm[j][i] = (bool) temp;
                }
            }
            fscanf(f,",");
            // Reverse i and j because we read csv opposite way
            // to the way the grid is set up (cartesian)
            
        }
        fscanf(f,"\n");  
    }
}

// Returns true if point collides with obstacle
bool pointCollision(point_t point, space_t *space) {
    return space->ogm[grid_lookup(point.x)][grid_lookup(point.y)];
}

// Given two points, step by distance EPSILON from p1 towards p2
point_t stepTowardsPoint(point_t p1, point_t p2) {
    float theta = atan2((p2.y-p1.y), (p2.x - p1.x));
    point_t newPoint;
    newPoint.x = p1.x + EPSILON*cos(theta);
    newPoint.y = p1.y + EPSILON*sin(theta);
    return newPoint;

}
// Return midpoint of a given edge
float splitDist(float p1, float p2) {
    return (p2 - p1) / SEGMENTS;
}

// Return true if point lies within grid, else false
bool pointInGrid(point_t point, point_t grid) {
    return (
        (grid.x <= point.x && point.x < grid.x + RESOLUTION) &&
        (grid.y <= point.y && point.y < grid.y + RESOLUTION)
    );
}

// Return true if edge intersects given grid, else false
bool segmentIntersectsGrid(edge_t edge, point_t grid) {
    float x_delta = splitDist(edge.p1.x, edge.p2.x);
    float y_delta = splitDist(edge.p1.y, edge.p2.y);
    
    bool collision = false;
    for (int i=0; i<=SEGMENTS; i++) {
        if (pointInGrid(
            (point_t) {
                .x = edge.p1.x + x_delta * i,
                .y = edge.p1.y + y_delta * i
            }, grid)) collision = true;
    }
    return collision;
}

bool edgeCollision(edge_t edge, space_t *space) {
    
    int min_x, max_x, min_y, max_y;
    
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

    if (XDIM - EPSILON < min_x) {
        min_x = XDIM-EPSILON;
    }
    if (YDIM - EPSILON < min_y) {
        min_y = YDIM-EPSILON;
    }

    max_x=min_x+EPSILON;
    max_y=min_y+EPSILON;

    // min_x = 0;
    // min_y=0;
    // max_x=XDIM;
    // max_y=YDIM;

    for (int i=min_x; i <= max_x; i++) {
        for (int j=min_y; j<=max_y; j++) {
            if (space->ogm[i][j]) {

                // Set up corner of grid
                point_t v = (point_t) {.x = i, .y = j};
                // Check if edge intersects with grid
                if (segmentIntersectsGrid(edge, v)) return true;
            }
        }
    }
    return false;
}

void fprint_point(FILE* f, point_t p) {
    fprintf(f, "%f, %f", p.x, p.y);
}
