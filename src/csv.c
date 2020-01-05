/*
* @Author: AnthonyKenny98
* @Date:   2020-01-05 10:32:59
* @Last Modified by:   AnthonyKenny98
* @Last Modified time: 2020-01-05 11:29:32
*/
#include <stdio.h>
#include <stdlib.h>


#define SIZE 4

int main(int argc, char *argv[]) {
    
    // Open File
    FILE *f = fopen("ogm.csv","r");
    
    // Define OGM
    int grid[SIZE][SIZE];
    
    // Fill out OGM
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            fscanf(f, "%i,", &grid[i][j]);
        }
        fscanf(f,"\n");  
    }
    
    // Print for Validation
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            printf("%i", grid[i][j]);
        }
        printf("\n");
    }
}