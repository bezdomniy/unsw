// util.h ... interface to various useful functions
// part of Multi-attribute Linear-hashed Files
// Functions that don't fit into one of the
//   obvious data types like File, Query, ...
// Last modified by John Shepherd, July 2019

#ifndef UTIL_H
#define UTIL_H 1

#define IS_BIG_ENDIAN (1 == *(unsigned char *)&(const int){1})

#include "defs.h"
#include "bits.h"

#define UINT_TYPE 0
#define CHAR_TYPE 1

typedef struct VectorRep *Vector;

void fatal(char *);
char *copyString(char *);


Vector init(size_t size, unsigned short type);
void freeVector(Vector vector);
Vector push(Vector vector, void* data);
void* get(Vector vector, unsigned int index);
void* pop(Vector vector);
size_t size(Vector vector);
unsigned int nextFreeSpot(Vector vector);
void _expand(Vector *vector);

#endif
