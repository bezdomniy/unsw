// query.c ... query scan functions
// part of Multi-attribute Linear-hashed Files
// Manage creating and using Query objects
// Last modified by John Shepherd, July 2019

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"
#include "page.h"

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

	for (i = 0; i < MAXCHVEC; i++) {
		a = chvec(r)[i].att;

		if (strcmp(new->query[a], "?") == 0) {
			new->unknown = setBit(new->unknown, i);
			new->known = unsetBit(new->known, i); //init all unknown in known as 0
		}
	}
	
	printf("%d\n",depth(r));
	new->unknown = getLower(new->unknown, depth(r));
	if (new->unknown < splitp(r)) new->unknown = getLower(new->unknown, depth(r)+1);

	char* tempPrint = malloc(sizeof(new->unknown) + 1);
	bitsString(new->unknown, tempPrint);
	printf("unknown: %s\n",tempPrint);

	

	new->curpage = new->known;
	new->pageBuffer = getPage(dataFile(new->rel), new->curpage);
	// putPage(dataFile(new->rel), new->curpage, new->pageBuffer);
	// printf("here\n");
	new->nextOverflowPage = pageOvflow(new->pageBuffer);
	new->curtup = 0;
	new->curtupIndex = 0;
	new->is_ovflow = 0;

	return new;
}

Status nextBucket(Query q) {
	int i;

	// if (q->is_ovflow) {
	// 	putPage(ovflowFile(q->rel), q->curpage, q->pageBuffer);
	// }
	// else {
	// 	putPage(dataFile(q->rel), q->curpage, q->pageBuffer);
	// }
	free(q->pageBuffer);
	
	for (i = 0; i < MAXCHVEC; i++) {
		if (bitIsSet(q->unknown, i)) {
			q->curpage = setBit(q->known, i);
			q->unknown = unsetBit(q->unknown, i);
			q->pageBuffer = getPage(dataFile(q->rel), q->curpage);
			q->nextOverflowPage = pageOvflow(q->pageBuffer);
			q->curtup = 0;
			q->curtupIndex = 0;
			q->is_ovflow = 0;
			break;
		}
	}
	if (i == MAXCHVEC) {
		printf("here\n");
		return 0; //no more pages
	}
	return 1;
}

int strcmpWithWildcard(char* str, Query q) {
	printf("query: %s\n", str);

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

	//int i;
	size_t stringSize;
	Tuple out;

	for (; q->curtupIndex < tuplesInPage; q->curtupIndex++) {
		stringSize = strlen(currentPageData + q->curtup);
		out = malloc(stringSize + 1);
		
		q->curtup += sprintf(out, "%s",currentPageData + q->curtup) + 1;

		printf("ovflow: %d. page: %d. ", q->is_ovflow, q->curpage);
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
	for (;;) {
		q->resultBuffer = getMatchingTupleFromCurrentPage(q);
		if (q->resultBuffer != NULL) {
			return q->resultBuffer;
		}
		else {
			if (q->nextOverflowPage  != NO_PAGE) {
				// printf("here1\n");
				// putPage(dataFile(q->rel), q->curpage, q->pageBuffer);
				// printf("here2\n");
				free(q->pageBuffer);
				
				q->curpage = q->nextOverflowPage;
				q->pageBuffer = getPage(ovflowFile(q->rel), q->curpage);
				q->nextOverflowPage = pageOvflow(q->pageBuffer);
				q->curtup = 0;
				q->curtupIndex = 0;
				q->is_ovflow = 1;

				q->resultBuffer = getMatchingTupleFromCurrentPage(q);
			}

			if (q->resultBuffer != NULL) {
				return q->resultBuffer;
			}
			else {
				if (nextBucket(q)) {
					printf("morepages\n");
					return getNextTuple(q);
				}
			}
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

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	// TODO
	freeVals(q->query, q->nvals);
}
