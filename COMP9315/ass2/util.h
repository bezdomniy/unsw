// util.h ... interface to various useful functions
// part of Multi-attribute Linear-hashed Files
// Functions that don't fit into one of the
//   obvious data types like File, Query, ...
// Last modified by John Shepherd, July 2019

#ifndef UTIL_H
#define UTIL_H 1

#include "defs.h"
#include "bits.h"

typedef struct VectorRep *Vector;

void fatal(char *);
char *copyString(char *);


Vector init(Vector vector, size_t length);
void freeVector(Vector vector);
void push(Vector vector, Bits data);
Bits get(Vector vector, unsigned int index);
unsigned int nextFreeSpot(Vector vector);

#endif
