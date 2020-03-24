#include "tools.h"



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

// float maxFloat3(float x, float y, float z) {
//     float max;
//     // Implementation 1
//     if (x > y) {
//         max = x;
//         if (z > x) max = z;
//     } else {
//         max = y;
//         if (z > y) max = z;
//     }
//     return max;
// }

// float minFloat3(float x, float y, float z) {
//     float min;
//     // Implementation 1
//     if (x < y) {
//         min = x;
//         if (z < x) min = z;
//     } else {
//         min = y;
//         if (z < y) min = z;
//     }
//     return min;
// }

// bool lineIntersectsPrism(edge_t edge, point_t prism_corner) {

//     float max = minFloat3(
//         lessThan(edge.p1.x, edge.p2.x, prism_corner.x, RESOLUTION),
//         lessThan(edge.p1.y, edge.p2.y, prism_corner.y, RESOLUTION),
//         lessThan(edge.p1.z, edge.p2.z, prism_corner.z, RESOLUTION)
//     );
//     float min = maxFloat3(
//         greaterThan(edge.p1.x, edge.p2.x, prism_corner.x),
//         greaterThan(edge.p1.y, edge.p2.y, prism_corner.y),
//         greaterThan(edge.p1.z, edge.p2.z, prism_corner.z)
//     );

//     return min < max;
// }


double value(double va, double vb, double t) {
    return (vb - va) * t + va;
}

bool lineIntersectsPlane(point_t A, point_t B, point_t C) {
    double t = (C.z - A.z) / (B.z - A.z);
    double p = value(A.x, B.x, t);
    double q = value(A.y, B.y, t);
    return ((C.x <= p) && (p <= C.x + RESOLUTION) && (C.y <= q) && (q <= C.y + RESOLUTION));
}

bool checkTwoFaces(point_t A, point_t B, point_t C) {
    if (lineIntersectsPlane(A, B, C)) return true;
    C.z += RESOLUTION;
    if (lineIntersectsPlane(A, B, C)) return true;
    return false;
}


bool lineIntersectsPrism(edge_t edge, point_t prism_corner) {


    point_t A, B, C;

    // Z Plane
    A = edge.p1;
    B = edge.p2;
    C = prism_corner;
    if (checkTwoFaces(A, B, C)) return true;

    // Y Plane
    A = (point_t) {.x = edge.p1.x, .y = edge.p1.z, .z = edge.p1.y};
    B = (point_t) {.x = edge.p2.x, .y = edge.p2.z, .z = edge.p2.y};
    C = (point_t) {.x = prism_corner.x, .y = prism_corner.z, .z = prism_corner.y};
    if (checkTwoFaces(A, B, C)) return true;

    // X Plane
    A = (point_t) {.x = edge.p1.z, .y = edge.p1.y, .z = edge.p1.x};
    B = (point_t) {.x = edge.p2.z, .y = edge.p2.y, .z = edge.p2.x};
    C = (point_t) {.x = prism_corner.z, .y = prism_corner.y, .z = prism_corner.z};
    if (checkTwoFaces(A, B, C)) return true;

    return false;
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
                    if (lineIntersectsPrism(edge, v)) return true;
                }
            }
        }
    }
    return false;
}

void fprint_point(FILE* f, point_t p) {
    fprintf(f, "%f, %f, %f", p.x, p.y, p.z);
}