// query.c ... query scan functions
// part of Multi-attribute Linear-hashed Files
// Manage creating and using Query objects
// Last modified by John Shepherd, July 2019

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"
#include "page.h"



#include <math.h> 
#define MIN(a,b) (((a)<(b))?(a):(b))

// A suggestion ... you can change however you like


struct QueryRep {
	Reln    rel;       // need to remember Relation info
	Bits    known;     // the hash value from MAH
	Bits    unknown;   // the unknown bits from MAH
	PageID  curpage;   // current page in scan
	PageID  nextOverflowPage;  

	int     is_ovflow; // are we in the overflow pages?
	Offset  curtup;    // offset of current tuple within page
	Count  curtupIndex;    // index of current tuple within page

	Page 	pageBuffer;
	Count 	curPageSize;

	Vector	bucketArray;
	// Count	unknownBits;
	Count   curBucket;

	char** 	query;
	Count 	nvals;
	Tuple	resultBuffer;

};

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q)
{
	Query new = malloc(sizeof(struct QueryRep));
	assert(new != NULL);

	new->rel = r;
	Bits h = tupleHash(r, q , 0);

	// if (depth(r) == 0) {
	// 	new->known = 1;
	// }
	// else {
	// 	new->known = getLower(h, depth(r));
	// 	if (new->known < splitp(r)) {
	// 		new->known = getLower(h, depth(r)+1);
	// 	} 
	// }

	new->known = getLower(h, depth(r) + 1);
	new->unknown = 0;

	new->nvals = nattrs(r);
	new->query = malloc(new->nvals*sizeof(char *));
	assert(new->query != NULL);
	tupleVals(q, new->query);

	int i;
	Byte a;
	// new->unknownBits = 0;

	for (i = 0; i < MAXCHVEC; i++) {
		a = chvec(r)[i].att;

		if (strcmp(new->query[a], "?") == 0) {
			new->unknown = setBit(new->unknown, i);
		}
	}


	new->unknown = getLower(new->unknown, depth(r) + 1);
	init(&new->bucketArray, pow(2, depth(r)+1), UINT_TYPE);

	generateBuckets(new, new->known, 0);

	void* nullCheckPtr = pop(new->bucketArray);
	if (nullCheckPtr) { 
		new->curpage = *(Bits*)nullCheckPtr;
	}
	else {
		fatal("empty hash list on init.\n");
	}


	new->pageBuffer = getPage(dataFile(new->rel), new->curpage);
	new->nextOverflowPage = pageOvflow(new->pageBuffer);
	new->curtup = 0;
	new->curtupIndex = 0;
	new->is_ovflow = 0;
	new->resultBuffer = NULL;
	

	return new;
}

void generateBuckets(Query q, Bits data, Count unknownIndex) {
	if (unknownIndex == MAXCHVEC) 
    { 
		if (data < npages(q->rel)){
			push(&q->bucketArray, &data);
		}
        return; 
    } 
  
    if (bitIsSet(q->unknown, unknownIndex))
    {	
        data = unsetBit(data, unknownIndex); 
		generateBuckets(q, data, unknownIndex + 1); 

        data = setBit(data, unknownIndex); 
        generateBuckets(q, data, unknownIndex + 1); 
    }
    else {
        generateBuckets(q, data, unknownIndex + 1); 
	}
}

Status nextBucket(Query q) {
	void* nullCheckPtr = pop(q->bucketArray);
	if (nullCheckPtr) {
		free(q->pageBuffer);
		q->curpage = *(Bits*)nullCheckPtr;

		q->pageBuffer = getPage(dataFile(q->rel), q->curpage);
		q->nextOverflowPage = pageOvflow(q->pageBuffer);
		q->curtup = 0;
		q->curtupIndex = 0;
		q->is_ovflow = 0;

		return 1;
	}
	else {
		return 0;
	}
}

int strcmpWithWildcard(char* str, Query q) {
	char **vals = malloc(q->nvals*sizeof(char *));
	
	tupleVals(str, vals);
	
	int i;

	for (i = 0; i < q->nvals; i++) {
		if ((strcmp(q->query[i], "?") != 0) && (strcmp(q->query[i], vals[i]) != 0)) {
			// printf("not match!\n");
			freeVals(vals, q->nvals);
			free(vals);
			return -1;
		}
	}
	// printf("match!\n");
	freeVals(vals, q->nvals);
	free(vals);
	return 0;
}

Tuple getMatchingTupleFromCurrentPage(Query q) {
	char* currentPageData = pageData(q->pageBuffer);
	Count tuplesInPage = pageNTuples(q->pageBuffer);

	size_t stringSize;
	Tuple out;

	while (q->curtupIndex < tuplesInPage) {
		q->curtupIndex++;
		stringSize = strlen(currentPageData + q->curtup);
		out = malloc(stringSize + 1);
		
		q->curtup += sprintf(out, "%s",currentPageData + q->curtup) + 1;

		if (strcmpWithWildcard(out, q) == 0) {
			free(q->resultBuffer);
			return out;
		}
		free(out);
	}
	if (q->resultBuffer) {
		free(q->resultBuffer);
	}
		
	return NULL;
}

// get next tuple during a scan

Tuple getNextTuple(Query q)
{
	q->resultBuffer = getMatchingTupleFromCurrentPage(q);
	if (q->resultBuffer != NULL) {
		return q->resultBuffer;
	}
	else if (q->nextOverflowPage  != NO_PAGE) {
		free(q->pageBuffer);
				
		q->curpage = q->nextOverflowPage;
		q->pageBuffer = getPage(ovflowFile(q->rel), q->curpage);
		q->nextOverflowPage = pageOvflow(q->pageBuffer);
		q->curtup = 0;
		q->curtupIndex = 0;
		q->is_ovflow = 1;

		return getNextTuple(q);
	}
	else {
		if (nextBucket(q)) {
			// printf("morepages\n");
			return getNextTuple(q);
		}
		else {
			free(q->pageBuffer);
			return NULL;
		}
	}

	return NULL;
}

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	freeVals(q->query, q->nvals);
	free(q->resultBuffer);
	free(q->query);
	free(q->bucketArray);

	free(q);
}
