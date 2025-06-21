#ifndef VECTOR_H
#define VECTOR_H

typedef struct{
    double x, y;
} Vector;

double vectorDistance(Vector v1, Vector v2);

#endif