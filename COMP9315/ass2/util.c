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
	size_t newSize = sizeof(Bits[size(vector)]) * 2;
	printf("expand\n");
	return realloc(vector, newSize);
}

struct VectorRep {
	unsigned int nextFreeSpot;
	size_t size;
	unsigned short type;
	char data[];
};

Vector init(size_t size, unsigned short type) {
	Vector vector;

	if (type == UINT_TYPE) {
		vector = malloc(sizeof(unsigned int[size]) + sizeof(struct VectorRep));
	}
	else if (type == CHAR_TYPE) {
		vector = malloc(sizeof(char[size]) + sizeof(struct VectorRep));
	}
	else {
		fatal("Invalid type.\n");
	}
	
	vector->nextFreeSpot = 0;
	vector->size = size;
	vector->type = type;
	return vector;
};

void freeVector(Vector vector) {
	free(vector->data);
};

Vector push(Vector vector, void* data) {
	// if (vector->type != UINT_TYPE) {
	// 	fatal("Invalid data type being pushed.\n");
	// }

	if (vector->nextFreeSpot == vector->size) {
		vector = _expand(vector);
	}

	if (vector->type == UINT_TYPE) {
		unsigned int n = *(unsigned int*)data;

		if (IS_BIG_ENDIAN) {
			vector->data[vector->nextFreeSpot + 3]     = (n >> 24) & 0xFF;
			vector->data[vector->nextFreeSpot + 2] = (n >> 16) & 0xFF;
			vector->data[vector->nextFreeSpot + 1] = (n >> 8) & 0xFF;
			vector->data[vector->nextFreeSpot] = n & 0xFF;
		} else {
			vector->data[vector->nextFreeSpot]     = (n >> 24) & 0xFF;
			vector->data[vector->nextFreeSpot + 1] = (n >> 16) & 0xFF;
			vector->data[vector->nextFreeSpot + 2] = (n >> 8) & 0xFF;
			vector->data[vector->nextFreeSpot + 3] = n & 0xFF;
		}


		vector->nextFreeSpot += sizeof(unsigned int);

	} else if (vector->type == CHAR_TYPE) {
		int i;
		for (i = 0; i < strlen((char*)data); i++) {
			vector->data[vector->nextFreeSpot + i] = ((char*)data)[i];
		}
		// vector->data[vector->nextFreeSpot] = *(char*)data;
		vector->nextFreeSpot += strlen((char*)data) + 1;

		//printf("in: %s, next spot: %d\n", (char*)data, (int)vector->nextFreeSpot);
	}
	else {
		fatal("Invalid type somehow in vector.\n");
	}

	return vector;
};

void* get(Vector vector, unsigned int index) {

	if (vector->type == UINT_TYPE) {
		// unsigned char bytes[4];
		
		// bytes[0] = vector->data[index * sizeof(unsigned int)]     ;
		// bytes[1] = vector->data[index * sizeof(unsigned int) + 1] ;
		// bytes[2] = vector->data[index * sizeof(unsigned int) + 2] ;
		// bytes[3] = vector->data[index * sizeof(unsigned int) + 3] ;

		// return bytes;
		return (void *)&vector->data[index * sizeof(unsigned int)]     ;
	} else if (vector->type == CHAR_TYPE) {
		return (void *)&vector->data[index];
	}
	else {
		fatal("Invalid type somehow in vector.\n");
	}

	return NULL;
};

unsigned int nextFreeSpot(Vector vector) {
	return vector->nextFreeSpot;
};

size_t size(Vector vector) {
	return vector->size;
}