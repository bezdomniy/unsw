// util.h ... interface to various useful functions
// part of Multi-attribute Linear-hashed Files
// Functions that don't fit into one of the
//   obvious data types like File, Query, ...
// Last modified by John Shepherd, July 2019

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include <math.h>

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
	size_t size;
	unsigned short type;
	char data[];
};

void _expand(Vector *vector, size_t tupleLen) {
	size_t newSize;

	unsigned int expansionFactor = (unsigned int)ceil(((double)(*vector)->nextFreeSpot + tupleLen)/(double)(*vector)->size);

	if ((*vector)->type == UINT_TYPE) {
		newSize = (sizeof(unsigned int[size((*vector))]) * expansionFactor) + sizeof(struct VectorRep);
	}
	else if ((*vector)->type == CHAR_TYPE) {
		newSize = (sizeof(char[size((*vector))]) * expansionFactor) + sizeof(struct VectorRep);
	}
	else {
		fatal("expand: Invalid type somehow in vector.\n");
	}  

	*vector = (Vector)realloc(*vector, newSize);
	if (*vector == NULL) {
		fatal("failed realloc.");
	}

	(*vector)->size *= expansionFactor;
}



void init(Vector *vector, size_t size, unsigned short type) {
	if (type == UINT_TYPE) {
		(*vector) = (Vector)malloc(sizeof(unsigned int[size]) + sizeof(struct VectorRep));
	}
	else if (type == CHAR_TYPE) {
		(*vector) = (Vector)malloc(sizeof(char[size]) + sizeof(struct VectorRep));
	}
	else {
		fatal("Invalid type.\n");
	}

	(*vector)->nextFreeSpot = 0;
	(*vector)->size = size;
	(*vector)->type = type;
}


void push(Vector *vector, void* data) {
	size_t tupleLen = sizeof(unsigned int);
	if ((*vector)->type == CHAR_TYPE)
		tupleLen = strlen((char*)data) + 1;

	// //Debug
	// if ((*vector)->type == UINT_TYPE)
	// 	printf("in: |%d|, next spot: %d\n", *(int*)data, (int)(*vector)->nextFreeSpot);
	// if ((*vector)->type == CHAR_TYPE)
	// 	printf("in: |%s|, next spot: %d\n", (char*)data, (int)(*vector)->nextFreeSpot);
	
	if ((*vector)->nextFreeSpot + tupleLen >= (*vector)->size) {
		_expand(vector, tupleLen);
	}

	if ((*vector)->type == UINT_TYPE) {
		unsigned int n = *(unsigned int*)data;

		if (IS_BIG_ENDIAN) {
			(*vector)->data[(*vector)->nextFreeSpot + 3]     = (n >> 24) & 0xFF;
			(*vector)->data[(*vector)->nextFreeSpot + 2] = (n >> 16) & 0xFF;
			(*vector)->data[(*vector)->nextFreeSpot + 1] = (n >> 8) & 0xFF;
			(*vector)->data[(*vector)->nextFreeSpot] = n & 0xFF;
		} else {
			(*vector)->data[(*vector)->nextFreeSpot]     = (n >> 24) & 0xFF;
			(*vector)->data[(*vector)->nextFreeSpot + 1] = (n >> 16) & 0xFF;
			(*vector)->data[(*vector)->nextFreeSpot + 2] = (n >> 8) & 0xFF;
			(*vector)->data[(*vector)->nextFreeSpot + 3] = n & 0xFF;
		}
	} else if ((*vector)->type == CHAR_TYPE) {
		strcpy((*vector)->data + (*vector)->nextFreeSpot, (char*)data);
	}
	else {
		fatal("Push: Invalid type somehow in vector.\n");
	}
	(*vector)->nextFreeSpot += tupleLen;
}

void* pop(Vector vector) {
	if (vector->type == UINT_TYPE) {

		int index = vector->nextFreeSpot- (unsigned int)sizeof(unsigned int);
		if (index < 0) {
			return NULL;
		}

		void* ret = (void *)&vector->data[index];
		vector->nextFreeSpot -= sizeof(unsigned int);
		return ret;
	} else if (vector->type == CHAR_TYPE) {
		fatal("Pop not implemented for char_type vector.\n");
	}
	else {
		fatal("Pop: Invalid type somehow in vector.\n");
	}

	return NULL;
}

void* get(Vector vector, unsigned int index) {
	if (vector->type == UINT_TYPE) {
		return (void *)&vector->data[index * sizeof(unsigned int)]     ;
	} else if (vector->type == CHAR_TYPE) {
		return (void *)&vector->data[index];
	}
	else {
		fatal("Invalid type somehow in vector.\n");
	}

	return NULL;
}

unsigned int nextFreeSpot(Vector vector) {
	return vector->nextFreeSpot;
}

size_t size(Vector vector) {
	return vector->size;
}