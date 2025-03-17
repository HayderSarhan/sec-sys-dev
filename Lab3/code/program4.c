#include<stdio.h>

#include<stdlib.h>

#include<string.h>


char* getString() {

    char message[100] = "Hello World!";

    char* ret = (char*)malloc(strlen(message) + 1);

    strcpy(ret, message);

    return ret;

}


void program4() {

    char* str = getString();

    printf("String: %s\n", str);

    free(str);

}


int main() {

    program4();

}
