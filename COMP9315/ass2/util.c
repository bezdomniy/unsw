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

void _expand(Vector *vector) {
	size_t newSize;
	// size_t oldSize;
	// unsigned int expansionFactor = 2;
	// if ( vector->nextFreeSpot - vector->size > vector->size) {

	// }

	unsigned int expansionFactor = (unsigned int)ceil((double)(*vector)->nextFreeSpot/(double)(*vector)->size);
	printf("expfact %d\n", expansionFactor);

	if ((*vector)->type == UINT_TYPE) {
		// oldSize = sizeof(unsigned int[size(vector)]) + sizeof(struct VectorRep);
		newSize = (sizeof(unsigned int[size((*vector))]) * expansionFactor) + sizeof(struct VectorRep);
	}
	else if ((*vector)->type == CHAR_TYPE) {
		// oldSize = sizeof(char[size(vector)]) + sizeof(struct VectorRep);
		newSize = (sizeof(char[size((*vector))]) * expansionFactor) + sizeof(struct VectorRep);
	}
	else {
		fatal("expand: Invalid type somehow in vector.\n");
	}

		printf("sizeb4 %d\n", (int)(*vector)->size);
	printf("size2b %d\n", (int)newSize);

	// Vector ret = malloc(newSize);
	// memcpy(ret, vector, oldSize);
	// free(vector);


	*vector = (Vector)realloc(*vector, newSize);
	
	(*vector)->size *= expansionFactor;
	printf("sizeaf %d\n", (int)(*vector)->size);
	if (*vector == NULL) {
		fatal("failed realloc.");
	}
	printf("expand\n");
	//return ret;
}



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
}

void freeVector(Vector vector) {
	free(vector->data);
}

Vector push(Vector vector, void* data) {
	if (vector->nextFreeSpot >= vector->size) {
		//vector = _expand(vector);
		_expand(&vector);
	}

	printf("%d type\n", vector->type);

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
		strcpy(vector->data + vector->nextFreeSpot, (char*)data);

		// printf("in: %s, next spot: %d\n", (char*)data, (int)vector->nextFreeSpot);
		// printf("in: %s, next spot: %d\n", vector->data +vector->nextFreeSpot, (int)vector->nextFreeSpot);

		vector->nextFreeSpot += strlen((char*)data) + 1;
	}
	else {
		// printf("%d type\n", vector->type);
		fatal("Push: Invalid type somehow in vector.\n");
	}

	return vector;
}

void* pop(Vector vector) {
	if (vector->type == UINT_TYPE) {
		void* ret = (void *)&vector->data[vector->nextFreeSpot - sizeof(unsigned int)];
		vector->nextFreeSpot -= sizeof(unsigned int);
		return ret;
	} else if (vector->type == CHAR_TYPE) {
		fatal("Pop not implemented for char_type vector.\n");
		// return (void *)&vector->data[index];
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