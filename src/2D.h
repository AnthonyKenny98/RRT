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

bool LineIntersectsLine(edge_t e1, edge_t e2) {
    double q = (e1.p1.y - e2.p1.y) * (e2.p2.x - e2.p1.x) - (e1.p1.x - e2.p1.x) * (e2.p2.y - e2.p1.y);
    double d = (e1.p2.x - e1.p1.x) * (e2.p2.y - e2.p1.y) - (e1.p2.y - e1.p1.y) * (e2.p2.x - e2.p1.x);

    if( d == 0 )
    {
        return false;
    }

    double r = q / d;

    q = (e1.p1.y - e2.p1.y) * (e1.p2.x - e1.p1.x) - (e1.p1.x - e2.p1.x) * (e1.p2.y - e1.p1.y);
    double s = q / d;

    if( r < 0 || r > 1 || s < 0 || s > 1 )
    {
        return false;
    }

    return true;
}

bool edgeCollisions(edge_t edge, space_t *space) {
    for (int i=0; i<XDIM/RESOLUTION; i++) {
        for (int j=0; j<YDIM/RESOLUTION; j++) {
            if (space->ogm[i][j]) {

                // Set up edges of grid
                point_t v1 = (point_t) {.x = i*RESOLUTION, .y = j*RESOLUTION};
                point_t v2 = (point_t) {.x = i*RESOLUTION, .y = j*RESOLUTION + RESOLUTION};
                point_t v3 = (point_t) {.x = i*RESOLUTION + RESOLUTION, .y = j*RESOLUTION + RESOLUTION};
                point_t v4 = (point_t) {.x = i*RESOLUTION + RESOLUTION, .y = j*RESOLUTION};
                
                if (LineIntersectsLine(edge, (edge_t) {.p1 = v1, .p2 = v2}) ||
                    LineIntersectsLine(edge, (edge_t) {.p1 = v2, .p2 = v3}) ||
                    LineIntersectsLine(edge, (edge_t) {.p1 = v3, .p2 = v4}) ||
                    LineIntersectsLine(edge, (edge_t) {.p1 = v4, .p2 = v1})) {
                    return true;
                }
            }
        }
    }
    return false;
}

void fprint_point(FILE* f, point_t p) {
    fprintf(f, "%f, %f", p.x, p.y);
}
