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

typedef point_t vector_t;

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

// Return true if point lies within grid, else false
bool pointInGrid(point_t point, point_t grid) {
    return (
        (grid.x <= point.x && point.x <= grid.x + RESOLUTION) &&
        (grid.y <= point.y && point.y <= grid.y + RESOLUTION) &&
        (grid.z <= point.z && point.z <= grid.z + RESOLUTION)
    );
}

// Return the vector of the two points
vector_t vector(point_t p1, point_t p2) {
    return (vector_t) {
        .x = p1.x - p2.x,
        .y = p1.y - p2.y,
        .z = p1.z - p2.z
    };
}

vector_t crossProduct(vector_t v1, vector_t v2) {
    return (vector_t) {
        .x = (v1.y * v2.z - v1.z * v2.y),
        .y = (v1.z * v2.x - v1.x * v2.z),
        .z = (v1.x * v2.y - v1.y * v2.x)
    };
}

float dotProduct(vector_t v1, point_t v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

float paramT(vector_t norm, float plane_d, edge_t e) {
    return ((plane_d - (norm.x*e.p1.x + norm.y*e.p1.y + norm.z*e.p1.z)) /
        (norm.x*(e.p2.x - e.p1.x) + norm.y*(e.p2.y - e.p1.y) + norm.z*(e.p2.z - e.p1.z))
    );
}

point_t pointOfIntersection(float T, edge_t edge) {
    return ( (point_t) {
        .x = edge.p1.x + T*(edge.p2.x - edge.p1.x),
        .y = edge.p1.y + T*(edge.p2.y - edge.p1.y),
        .z = edge.p1.z + T*(edge.p2.z - edge.p1.z),
    });
}

bool pointOnSegment(point_t p, edge_t e) {
    return (
        ((min(e.p1.x, e.p2.x) <= p.x) && (p.x <= max(e.p1.x, e.p2.x))) &&
        ((min(e.p1.y, e.p2.y) <= p.y) && (p.y <= max(e.p1.y, e.p2.y))) &&
        ((min(e.p1.z, e.p2.z) <= p.z) && (p.z <= max(e.p1.z, e.p2.z)))
    );
}

bool pointOnFace(point_t p, point_t grid) {
    return (
        ((grid.x <= p.x) && (p.x <= grid.x + RESOLUTION)) &&
        ((grid.y <= p.y) && (p.y <= grid.y + RESOLUTION)) &&
        ((grid.y <= p.y) && (p.y <= grid.y + RESOLUTION))
    );
}

bool segmentIntersectsFace(edge_t edge, point_t face) {
    // Setup 3 points on the associated plane
    point_t P, Q, R;
    P = face;
    Q = face;
    Q.x+=RESOLUTION;
    R = face;
    R.y+=RESOLUTION;

    // Get vectors defining plane
    vector_t PQ = vector(P, Q);
    vector_t PR = vector(P, R);

    // Normal to the plane
    vector_t norm = crossProduct(PQ, PR);

    // dot product of normal and point on plane
    float dot = dotProduct(norm, P);

    // Parameter T shared by plane and line
    float T = paramT(norm, dot, edge);

    // Point of intersection
    point_t POI = pointOfIntersection(T, edge);
    // printf("        POI = (%f,%f, %f)\n", POI.x, POI.y, POI.z);

    // Check if point is within bounds of edge
    bool pointOnEdge = pointOnSegment(POI, edge);
    // printf("        Point on segment = %d\n", pointOnEdge);

    // Check if point is within bounds of face
    bool pointOnface = pointOnFace(POI, P);
    // printf("        point on face = %d\n", pointOnface);
    // printf("    segmentIntersectsFace= %d\n", (pointOnEdge && pointOnface));

    return pointOnEdge && pointOnface;
}

bool segmentIntersectsGrid(edge_t edge, point_t grid) {
    bool segmentIntersectsAnyFace = (
        // Z Plane
        segmentIntersectsFace(edge, grid) ||
        segmentIntersectsFace(edge, 
            (point_t) {.x=grid.x, .y=grid.y, .z=grid.z+RESOLUTION}) ||
        
        // Y Plane
        segmentIntersectsFace(//edge,
            (edge_t) {
                .p1=(point_t) {.x=edge.p1.x, .y=edge.p1.z, .z=edge.p1.y},
                .p2=(point_t) {.x=edge.p2.x, .y=edge.p2.z, .z=edge.p2.y},
            }, 
            (point_t) {.x=grid.x, .y=grid.z, .z=grid.y}) ||
        segmentIntersectsFace(//edge,
            (edge_t) {
                .p1=(point_t) {.x=edge.p1.x, .y=edge.p1.z, .z=edge.p1.y},
                .p2=(point_t) {.x=edge.p2.x, .y=edge.p2.z, .z=edge.p2.y},
            }, 
            (point_t) {.x=grid.x, .y=grid.z, .z=grid.y+RESOLUTION}) ||

        // X Plane
        segmentIntersectsFace(//edge,
            (edge_t) {
                .p1=(point_t) {.x=edge.p1.z, .y=edge.p1.y, .z=edge.p1.x},
                .p2=(point_t) {.x=edge.p2.z, .y=edge.p2.y, .z=edge.p2.x},
            }, 
            (point_t) {.x=grid.z, .y=grid.y, .z=grid.x}) ||
        segmentIntersectsFace(//edge,
            (edge_t) {
                .p1=(point_t) {.x=edge.p1.z, .y=edge.p1.y, .z=edge.p1.x},
                .p2=(point_t) {.x=edge.p2.z, .y=edge.p2.y, .z=edge.p2.x},
            }, 
            (point_t) {.x=grid.z, .y=grid.y, .z=grid.x+RESOLUTION})
    );
    bool bothEndPointsInGrid = (
        pointInGrid(edge.p1, grid) && pointInGrid(edge.p2, grid)
    );
    return segmentIntersectsAnyFace || bothEndPointsInGrid;
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

point_t getStartPoint() {return getRandomPoint();}
point_t getGoalPoint() {return getRandomPoint();}

void fprint_point(FILE* f, point_t p) {
    fprintf(f, "%f, %f, %f", p.x, p.y, p.z);
}