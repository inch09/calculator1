#include <stdio.h>
#include <math.h>

int factorial(int n);

int main(){

    printf("%d", factorial(6));

    return 0;
}

int factorial(int n){

    if(n == 1){
        return 1;
    }
    return factorial(n - 1) * n;

}