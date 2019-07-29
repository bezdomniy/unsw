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


struct VectorRep {
	unsigned int nextFreeSpot;
	//size_t length;
	Bits data[];
};

Vector init(Vector vector, size_t length) {
	vector = malloc(sizeof(Bits[length]) + sizeof(struct VectorRep));
	vector->nextFreeSpot = 0;
	//bucketArray->length = length;
	return vector;
};

void freeVector(Vector vector) {
	free(vector->data);
};

void push(Vector vector, Bits data) {
	vector->data[vector->nextFreeSpot] = data;
	vector->nextFreeSpot++;
};

Bits get(Vector vector, unsigned int index) {
	return vector->data[index];
};

unsigned int nextFreeSpot(Vector vector) {
	return vector->nextFreeSpot;
};