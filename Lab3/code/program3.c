#include<stdio.h>

#include<stdlib.h>

#include<string.h>


void* program3(unsigned N) {

    void *arr = malloc(N * sizeof(*arr));

    if((N < 1) || (arr == NULL)) {

        printf("%s\n", "Memory allocation failed!");

        free(arr);

        return NULL;

    }

    printf("%s\n", "Memory allocation success!");

    return arr;

}


int main() {

    int* arr = (int*)program3(4); // Should typically succeed

    // When condition is true, to test that we dont have memory leaks
    // int* arr = (int*)program3(0);

    free(arr);

}