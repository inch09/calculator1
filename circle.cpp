#include <stdio.h>
#include <assert.h>
#include <math.h>

bool isEqual(double a, double b);

struct Point{
    int x;
    int y;
};

int main(){
    FILE* file = fopen("circle.txt", "w");
    assert(file);
    const size_t side = 15;
    const double scaleCoeff = 1.31;
    const size_t sideLenX = side;
    const size_t sideLenY = side;//sideLenX / scaleCoeff + 1;

    printf("%lf", scaleCoeff);
    int arr[sideLenX * sideLenY] = {};

    double xCenter = (sideLenX + 1) / 2;
    double yCenter = (sideLenY + 1) / 2;
    double radius =  sideLenX  / 2;

    for(size_t y = 1; y <= sideLenY; y++){
        for(size_t x = 1; x <= sideLenX; x++){
            if(((xCenter - x) * (xCenter - x) + (yCenter - y)  * (yCenter - y)  * (scaleCoeff * scaleCoeff)) <= radius * radius){
                arr[sideLenX * (y - 1) + x - 1] = 1;
            }
            else{
                arr[sideLenX * (y - 1) + x - 1] = 0;
            }
        }
    }

    for(size_t y = 1; y <= sideLenY; y++){
        for(size_t x = 1; x <= sideLenX; x++){
            if(arr[sideLenX * (y - 1) + x - 1] == 1){
                fprintf(file, "# ");
            }
            else{
                fprintf(file, ". ");
            }
        }
        fprintf(file, "\n");
    }

    for(size_t y = 1; y <= sideLenY; y++){
        for(size_t x = 1; x <= sideLenX; x++){
            if(arr[sideLenX * (y - 1) + x - 1] == 1){
                printf("# ");
            }
            else{
                printf(". ");
            }
        }
        printf("\n");
    }

    fclose(file);
    return 0;
}

bool isEqual(double a, double b){
    const double epsilon = 0.0001;
    return fabs(a - b) < epsilon;
}