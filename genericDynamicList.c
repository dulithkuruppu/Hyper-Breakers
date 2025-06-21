#include "genericDynamicList.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define CAPACITY_MULTIPLICATION 2
#define INIT_CAPACITY 10


struct dynList {
    dynElem* data;
    int capacity;
    int length;
    freeMemfunct freeFunct;
    printElemFunct printFunct;
};

dynList createDynList(freeMemfunct fFunct, printElemFunct pFunct) {
    dynList new = malloc(sizeof(struct dynList));
    new -> capacity = INIT_CAPACITY;
    new -> data = malloc(new->capacity * sizeof(dynElem));
    new -> length = 0;
    new -> freeFunct = fFunct;
    new -> printFunct = pFunct;
    return new;
}

dynElem getElem(dynList dList, int index) {
    assert(index >= 0 && index < (dList -> length));
    return dList->data[index];
}

int getIndexOf(dynList dList, dynElem element){
    for (int i = 0; i < getLengthDynList(dList); i++){
        if (dList->data[i] == element){
            return i;
        }
    }
    return -1;
}

dynElem *getData(dynList dList) {
    return dList -> data;
}


dynElem removeAtIndexDynList(dynList dList, int index) {
    
    assert(index >= 0 && index < (dList -> length));
    dynElem output = getElem(dList,index);
    for(int i = index+1; i < (dList -> length); i++) {
        dList -> data[i-1] = dList -> data[i];
    }
    dList -> length--;
    dList -> data[dList -> length]  = 0;
    return output;
}

dynElem removeDynList(dynList dList, dynElem element){
    return removeAtIndexDynList(dList,getIndexOf(dList,element));
}

int getLengthDynList(dynList dList) {
    return dList->length;
}

void increaseCap(dynList dList) {
    if(dList -> length >= dList-> capacity) {
        dList -> capacity *= CAPACITY_MULTIPLICATION;
        dList -> data = realloc(dList -> data, sizeof(dynElem) * (dList -> capacity));
        assert(dList->data != NULL);
    }
}

void insertDynList(dynList dList,dynElem new){
    increaseCap(dList);
    dList -> data[dList -> length] = new;
    dList -> length++;
}


void freeDynList(dynList dList) {
    for (int i = 0; i < (dList -> length); i ++){
        if ((dList -> freeFunct) == NULL){
            free((dList -> data[i]));
        } else {
            (dList -> freeFunct) (dList -> data[i]);
        }
    }
    free(dList -> data);
    free(dList);
}

void printDynList(dynList dList) {
    assert(dList -> printFunct != NULL);
    putchar('[');
    for(int i = 0; i< dList -> length; i++) {
        if(i>0) {
            printf(", ");
        }
        (dList -> printFunct) (getElem(dList,i));
    }
    printf("]\n");

}



