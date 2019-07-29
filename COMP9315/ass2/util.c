// util.h ... interface to various useful functions
// part of Multi-attribute Linear-hashed Files
// Functions that don't fit into one of the
//   obvious data types like File, Query, ...
// Last modified by John Shepherd, July 2019

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

void fatal(char *msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(1);
}

char *copyString(char *str)
{
	char *new = malloc(strlen(str)+1);
	strcpy(new, str);
	return new;
}

Vector _expand(Vector vector) {
	size_t newSize = sizeof(Bits[length(vector)]) * 2;
	printf("expand\n");
	return realloc(vector, newSize);
}

struct VectorRep {
	unsigned int nextFreeSpot;
	size_t length;
	Bits data[];
};

Vector init(size_t length) {
	Vector vector = malloc(sizeof(Bits[length]) + sizeof(struct VectorRep));
	vector->nextFreeSpot = 0;
	vector->length = length;
	return vector;
};

void freeVector(Vector vector) {
	free(vector->data);
};

Vector push(Vector vector, Bits data) {
	if (vector->nextFreeSpot == vector->length) {
		vector = _expand(vector);
	}

	vector->data[vector->nextFreeSpot] = data;
	vector->nextFreeSpot++;

	return vector;
};

Bits get(Vector vector, unsigned int index) {
	return vector->data[index];
};

unsigned int nextFreeSpot(Vector vector) {
	return vector->nextFreeSpot;
};

size_t length(Vector vector) {
	return vector->length;
}