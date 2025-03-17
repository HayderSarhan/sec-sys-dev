#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

// Returns the index where the key should be stored
int HashIndex(const char* key) {

    // Uninitialized variable (CWE-457)
    //int sum;

    int sum = 0;

    // Causes infinite loop (CWE-835)
    // for (char* c = key; c; c++) {
    //     sum += *c;
    // }

    for (const char* c = key; *c; c++) {
        sum += *c;
    }

    return sum % MAP_MAX;
}

// Allocates memory for the HashMap
HashMap* HashInit() {

    HashMap* map = malloc(sizeof(HashMap));
    if (!map)
        return NULL;
    memset(map, 0, sizeof(HashMap));

    return map;
}

// Inserts PairValue into the map, if the value exists, increase ValueCount
void HashAdd(HashMap *map, PairValue *value) {

    // Doesn't check if map or value are NULL (CWE-476)
    if(!map || !value)
        return;

    // Check if the key already exists in the map, if so, increase the ValueCount
    PairValue* checkHash = HashFind(map, value->KeyName);
    if (checkHash) {
        checkHash->ValueCount += value->ValueCount;
        return;
    }

    int idx = HashIndex(value->KeyName);
    value->Next = map->data[idx];
    map->data[idx] = value;
}

// Returns PairValue from the map if a given key is found
PairValue* HashFind(HashMap *map, const char* key) {

    // Doesn't check for NULL values (CWE-476)
    if (!map || !key)
        return NULL;

    unsigned idx = HashIndex(key);

    for(PairValue* val = map->data[idx]; val != NULL; val = val->Next) {

        // strcmp returns 0 on match (CWE-570)/(CWE-697)
        // if (strcmp(val->KeyName, key))
        //     return val;

        if (strcmp(val->KeyName, key) == 0)
            return val;
    }

    return NULL;
}

// Deletes the entry with the given key from the map
void HashDelete(HashMap *map, const char* key) {
    unsigned idx = HashIndex(key);

    for(PairValue* val = map->data[idx], *prev = NULL; val != NULL; prev = val, val = val->Next) {

        // strcmp returns 0 on match (CWE-570)/(CWE-697)
        // if (strcmp(val->KeyName, key)) {
        //     if (prev)
        //         prev->Next = val->Next;
        //     else
        //         map->data[idx] = val->Next;
        // }

        if (strcmp(val->KeyName, key) == 0) {
            if (prev)
                prev->Next = val->Next;
            else
                map->data[idx] = val->Next;
        }
    }
}

// Prints all content of the map
void HashDump(HashMap *map) {
    for(unsigned i = 0; i < MAP_MAX; i++) {
        for(PairValue* val = map->data[i]; val != NULL; val = val->Next) {

            // No format was specified (CWE-134)
            //printf(val->KeyName);

            printf("%s ", val->KeyName);
        }
    }
    printf("\n");
}


int main() {
    HashMap* map = HashInit();
    printf("%s\n", "HashInit() Successful");

    PairValue pv1 = { .KeyName = "test_key", .ValueCount = 1, .Next = NULL };
    PairValue pv2 = { .KeyName = "other_key", .ValueCount = 1, .Next = NULL };

    printf("HashAdd(map, '%s')\n", pv1.KeyName);
    HashAdd(map, &pv1);

    printf("HashAdd(map, '%s')\n", pv1.KeyName);
    HashAdd(map, &pv1);

    printf("HashAdd(map, '%s')\n", pv2.KeyName);
    HashAdd(map, &pv2);

    printf("HashFind(map, %s) = ", pv1.KeyName);
    PairValue* result = HashFind(map, pv1.KeyName);
    if(result) {
        printf("{'%s': %d}\n", result->KeyName, result->ValueCount);
    }
    else {
        printf("%s\n", "Not found");
    }

    printf("%s", "HashDump(map) = ");
    HashDump(map);

    printf("HashDelete(map, '%s')\n", pv1.KeyName);
    HashDelete(map, pv1.KeyName);

    printf("HashFind(map, %s) = ", pv1.KeyName);
    result = HashFind(map, pv1.KeyName);
    if(result) {
        printf("{'%s': %d}\n", result->KeyName, result->ValueCount);
    }
    else {
        printf("%s\n", "Not found");
    }

    printf("%s", "HashDump(map) = ");
    HashDump(map);

    free(map);
}
