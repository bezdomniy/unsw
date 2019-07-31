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
	Count	unknownBits;
	Count   curBucket;

	char** 	query;
	Count 	nvals;
	Tuple	resultBuffer;

};

void test1() {
	Vector tuplesInBucket;
	
	init(&tuplesInBucket, 1, CHAR_TYPE);

	push(&tuplesInBucket, "abc");
	push(&tuplesInBucket, "xz");
	push(&tuplesInBucket, "mofo");
	
	// int i;
	// for(i = 0; i < nextFreeSpot(tuplesInBucket); i++) {
	// 	printf("%s\n", (char*)get(tuplesInBucket, i));
	// }

	int i = 0;
	char* buf ;
	while (i < nextFreeSpot(tuplesInBucket)) {
		buf = (char*)get(tuplesInBucket, i);
		printf("%d, %s\n", i, buf);
		i+= strlen(buf) + 1;
	}

	//freeVector(tuplesInBucket);
	exit(0);
}

void test2() {
	Vector tuplesInBucket;
	
	init(&tuplesInBucket, 1, UINT_TYPE);

	unsigned int num = 1;

	push(&tuplesInBucket, &num);
	// num = 2;
	// push(&tuplesInBucket, &num);
	// num = 3;
	// push(&tuplesInBucket, &num);

	unsigned int out = 0;
	int i = 0;
	while (1) {
		void* nullCheckPtr = pop(tuplesInBucket);
		if (!nullCheckPtr) { //reuse a page, or add one if you can't
			out = 0;
			break;
		}
		else {
			out = *(unsigned int*)nullCheckPtr;
			printf("out: %d\n", out);
		}
		
		i+=sizeof(unsigned int);
	}

	
	// int i;
	// for(i = 0; i < nextFreeSpot(tuplesInBucket); i++) {
	// 	printf("%s\n", (char*)get(tuplesInBucket, i));
	// }

	// int i = 0;
	// char* buf ;
	// while (i < nextFreeSpot(tuplesInBucket)) {
	// 	buf = (char*)get(tuplesInBucket, i);
	// 	printf("%d, %s\n", i, buf);
	// 	i+= strlen(buf) + 1;
	// }

	//freeVector(tuplesInBucket);
	exit(0);
}

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q)
{
	test2(); 

	Query new = malloc(sizeof(struct QueryRep));
	assert(new != NULL);
	// TODO
	// Partial algorithm:
	// form known bits from known attributes
	// form unknown bits from '?' attributes
	// compute PageID of first page
	//   using known bits and first "unknown" value
	// set all values in QueryRep object

	new->rel = r;
	//new->queryString = copyString(q); //TODO: check if i need to copy
	Bits h = tupleHash(r, q);

	if (depth(r) == 0)
		new->known = 1;
	else {
		new->known = getLower(h, depth(r));
		if (new->known < splitp(r)) new->known = getLower(h, depth(r)+1);
	}


	new->unknown = 0;

	new->nvals = nattrs(r);
	new->query = malloc(new->nvals*sizeof(char *));
	assert(new->query != NULL);
	tupleVals(q, new->query);

	int i;
	Byte a;
	new->unknownBits = 0;

	for (i = 0; i < MAXCHVEC; i++) {
		a = chvec(r)[i].att;

		if (strcmp(new->query[a], "?") == 0) {
			new->unknown = setBit(new->unknown, i);
			//new->known = unsetBit(new->known, i); //init all unknown in known as 0
			new->unknownBits++;
		}
	}

	// printf("%d\n",depth(r));
	new->unknown = getLower(new->unknown, depth(r));
	if (new->unknown < splitp(r)) new->unknown = getLower(new->unknown, depth(r)+1);

	new->unknownBits = MIN(new->unknownBits, depth(r));

	// printf("unknownBits: %d\n",new->unknownBits);

	init(&new->bucketArray, pow(2, new->unknownBits) * sizeof(Bits), UINT_TYPE);

	generateBuckets(new, new->known, 0);

	char* tempPrint;
	for (i = 0; i < nextFreeSpot(new->bucketArray) / sizeof(Bits); i++) {
				
		tempPrint = malloc(MAXCHVEC + 1);
		bitsString(*(Bits*)get(new->bucketArray, i), tempPrint);
		printf("bitstring: %s\n",tempPrint);
		free(tempPrint);
	}


	new->curBucket = 0;
	new->curpage = *(Bits*)get(new->bucketArray, new->curBucket);

		// char* tempPrint;
		// tempPrint = malloc(MAXCHVEC + 1);
		// bitsString(new->unknown, tempPrint);
		// printf("unknown: %s\n",tempPrint);
		// free(tempPrint);

	new->pageBuffer = getPage(dataFile(new->rel), new->curpage);
	// putPage(dataFile(new->rel), new->curpage, new->pageBuffer);
	// printf("depth %d\n", depth(r)+1);
	new->nextOverflowPage = pageOvflow(new->pageBuffer);
	new->curtup = 0;
	new->curtupIndex = 0;
	new->is_ovflow = 0;
	

	return new;
}

void generateBuckets(Query q, Bits data, Count unknownIndex) {
	if (unknownIndex == MAXCHVEC) 
    { 
		// char* tempPrint;
		// tempPrint = malloc(MAXCHVEC + 1);
		// bitsString(data, tempPrint);
		// printf("bitstring: %s\n",tempPrint);
		// free(tempPrint);

		// q->bucketArray = push(q->bucketArray, (void*)&data);
		push(&q->bucketArray, (void*)&data);

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
	q->curBucket++;
	if (q->curBucket < nextFreeSpot(q->bucketArray)/sizeof(Bits)) {
		
		free(q->pageBuffer);

		q->curpage = *(Bits*)get(q->bucketArray,q->curBucket);
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
	// printf("query: %s\n", str);

	char **vals = malloc(q->nvals*sizeof(char *));
	
	tupleVals(str, vals);
	
	int i;

	

	for (i = 0; i < q->nvals; i++) {
		if ((strcmp(q->query[i], "?") != 0) && (strcmp(q->query[i], vals[i]) != 0)) {
			// printf("not match!\n");
			freeVals(vals, q->nvals);
			return -1;
		}
			// printf("next\n");
	}
	// printf("match!\n");
	freeVals(vals, q->nvals);
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

		//printf("ovflow: %d. page: %d. \n", q->is_ovflow, q->curpage);

		if (strcmpWithWildcard(out, q) == 0) {
			free(q->resultBuffer);
			return out;
		}
		free(out);
	}
	free(q->resultBuffer);
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
			return NULL;
		}
	}


	// printf("%s\n", out);

	// TODO
	// Partial algorithm:
	// if (more tuples in current page)
	//    get next matching tuple from current page
	// else if (current page has overflow)
	//    move to overflow page
	//    grab first matching tuple from page
	// else
	//    move to "next" bucket
	//    grab first matching tuple from data page
	// endif
	// if (current page has no matching tuples)
	//    go to next page (try again)
	// endif
	return NULL;
}

// Tuple getNextTuple(Query q)
// {
// 	for (;;) {
// 		q->resultBuffer = getMatchingTupleFromCurrentPage(q);
// 		if (q->resultBuffer != NULL) {
// 			return q->resultBuffer;
// 		}
// 		else {
// 			if (q->nextOverflowPage  != NO_PAGE) {
// 				// printf("here1\n");
// 				// putPage(dataFile(q->rel), q->curpage, q->pageBuffer);
// 				// printf("here2\n");
// 				free(q->pageBuffer);
				
// 				q->curpage = q->nextOverflowPage;
// 				q->pageBuffer = getPage(ovflowFile(q->rel), q->curpage);
// 				q->nextOverflowPage = pageOvflow(q->pageBuffer);
// 				q->curtup = 0;
// 				q->curtupIndex = 0;
// 				q->is_ovflow = 1;

// 				q->resultBuffer = getMatchingTupleFromCurrentPage(q);
// 			}

// 			if (q->resultBuffer != NULL) {
// 				return q->resultBuffer;
// 			}
// 			else {
// 				if (nextBucket(q)) {
// 					// printf("morepages\n");
// 					return getNextTuple(q);
// 				}
// 				else {
// 					return NULL;
// 				}
// 			}
// 		}
// 	}


// 	// printf("%s\n", out);

// 	// TODO
// 	// Partial algorithm:
// 	// if (more tuples in current page)
// 	//    get next matching tuple from current page
// 	// else if (current page has overflow)
// 	//    move to overflow page
// 	//    grab first matching tuple from page
// 	// else
// 	//    move to "next" bucket
// 	//    grab first matching tuple from data page
// 	// endif
// 	// if (current page has no matching tuples)
// 	//    go to next page (try again)
// 	// endif
// 	return NULL;
// }

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	// TODO
	freeVals(q->query, q->nvals);
}
