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

	int     is_ovflow; // are we in the overflow pages?
	Offset  curtup;    // offset of current tuple within page

	Page 	pageBuffer;
	Count 	curPageSize;

	char* 	queryString;
	Tuple	resultBuffer;
	//TODO
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
	new->queryString = copyString(q); //TODO: check if i need to copy
	Bits h = tupleHash(r, q);

	if (depth(r) == 0)
		new->known = 1;
	else {
		new->known = getLower(h, depth(r));
		if (new->known < splitp(r)) new->known = getLower(h, depth(r)+1);
	}

	new->is_ovflow = 0;
	new->curtup = 0;

	new->unknown = 0;

	Count nvals = nattrs(r);
	char **vals = malloc(nvals*sizeof(char *));
	assert(vals != NULL);
	tupleVals(q, vals);

	int i;
	Byte a;

	for (i = 0; i < MAXCHVEC; i++) {
		a = chvec(r)[i].att;

		if (strcmp(vals[a], "?") == 0) {
			new->unknown = setBit(new->unknown, i);
			new->known = unsetBit(new->known, i); //init all unknown in known as 0
		}
			
	}

	freeVals(vals, nvals);

	new->curpage = new->known;
	new->pageBuffer = getPage(dataFile(new->rel), new->curpage);
	new->curtup = 0;

	return new;
}

Status nextPage(Query q) {
	int i;

	putPage(dataFile(q->rel), q->curpage, q->pageBuffer);
	for (i = 0; i < MAXCHVEC; i++) {
		if (!bitIsSet(q->known, i) && bitIsSet(q->unknown, i)) {
			q->curpage = setBit(q->known, i);
			q->known = q->curpage;
			q->pageBuffer = getPage(dataFile(q->rel), q->curpage);
			q->curtup = 0;
			break;
		}
	}
	if (i == MAXCHVEC) {
		return 1; //no more pages
	}
	return 0;
}

int strcmpWithWildcard(char* a, char* b) {
	return strcmp(a, b); //TODO: add wildcard 
}

Tuple getMatchingTupleFromCurrentPage(Query q) {
	char* currentPageData = pageData(q->pageBuffer);
	Count tuplesInPage = pageNTuples(q->pageBuffer);

	int i;
	size_t stringSize;
	Tuple out;

	for (i = 0; i < tuplesInPage; i++) {
		stringSize = strlen(currentPageData + q->curtup);
		out = malloc(stringSize + 1);
		
		q->curtup += sprintf(out, "%s",currentPageData + q->curtup) + 1;
	
		if (strcmpWithWildcard(out, q->queryString) == 0) {
			return out;
		}
		free(out);
	}
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
			q->curpage = pageOvflow(q->pageBuffer);

			if (q->curpage != NO_PAGE) {
				q->pageBuffer = getPage(ovflowFile(q->rel), q->curpage);
				q->curtup = 0;
				q->is_ovflow = 1;

				q->resultBuffer = getMatchingTupleFromCurrentPage(q);
			}

			if (q->resultBuffer != NULL) {
				return q->resultBuffer;
			}
			else {
				int i;
				for (i = 0; i < MAXCHVEC; i++) {
					if (bitIsSet(q->unknown, i)) { //move unknown bit to curpage to try other permutation
						q->curpage = setBit(q->curpage, i);
						q->unknown = unsetBit(q->unknown, i);
					}
				}

				if (i == MAXCHVEC) {
					break;
				}

				q->curtup = 0;
				q->is_ovflow = 0;
				q->pageBuffer = getPage(ovflowFile(q->rel), q->curpage);
				q->resultBuffer = getMatchingTupleFromCurrentPage(q);

				if (q->resultBuffer != NULL) {
					return q->resultBuffer;
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
}
