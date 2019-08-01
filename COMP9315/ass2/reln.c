// reln.c ... functions on Relations
// part of Multi-attribute Linear-hashed Files
// Last modified by John Shepherd, July 2019

#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tuple.h"
#include "chvec.h"
#include "bits.h"
#include "hash.h"

#include <math.h> 

#define HEADERSIZE (3*sizeof(Count)+sizeof(Offset))

struct RelnRep {
	Count  nattrs; // number of attributes
	Count  depth;  // depth of main data file
	Offset sp;     // split pointer
    Count  npages; // number of main data pages
    Count  ntups;  // total number of tuples
	ChVec  cv;     // choice vector
	char   mode;   // open for read/write
	FILE  *info;   // handle on info file
	FILE  *data;   // handle on data file
	FILE  *ovflow; // handle on ovflow file
};

// create a new relation (three files)

Status newRelation(char *name, Count nattrs, Count npages, Count d, char *cv)
{
    char fname[MAXFILENAME];
	Reln r = malloc(sizeof(struct RelnRep));
	r->nattrs = nattrs; r->depth = d; r->sp = 0;
	r->npages = npages; r->ntups = 0; r->mode = 'w';
	assert(r != NULL);
	if (parseChVec(r, cv, r->cv) != OK) return ~OK;
	sprintf(fname,"%s.info",name);
	r->info = fopen(fname,"w");
	assert(r->info != NULL);
	sprintf(fname,"%s.data",name);
	r->data = fopen(fname,"w");
	assert(r->data != NULL);
	sprintf(fname,"%s.ovflow",name);
	r->ovflow = fopen(fname,"w");
	assert(r->ovflow != NULL);
	int i;
	for (i = 0; i < npages; i++) addPage(r->data);
	closeRelation(r);
	return 0;
}

// check whether a relation already exists

Bool existsRelation(char *name)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.info",name);
	FILE *f = fopen(fname,"r");
	if (f == NULL)
		return FALSE;
	else {
		fclose(f);
		return TRUE;
	}
}

// set up a relation descriptor from relation name
// open files, reads information from rel.info

Reln openRelation(char *name, char *mode)
{
	Reln r;
	r = malloc(sizeof(struct RelnRep));
	assert(r != NULL);
	char fname[MAXFILENAME];
	sprintf(fname,"%s.info",name);
	r->info = fopen(fname,mode);
	assert(r->info != NULL);
	sprintf(fname,"%s.data",name);
	r->data = fopen(fname,mode);
	assert(r->data != NULL);
	sprintf(fname,"%s.ovflow",name);
	r->ovflow = fopen(fname,mode);
	assert(r->ovflow != NULL);
	// Naughty: assumes Count and Offset are the same size
	int n = fread(r, sizeof(Count), 5, r->info);
	assert(n == 5);
	n = fread(r->cv, sizeof(ChVecItem), MAXCHVEC, r->info);
	assert(n == MAXCHVEC);
	r->mode = (mode[0] == 'w' || mode[1] =='+') ? 'w' : 'r';
	return r;
}

// release files and descriptor for an open relation
// copy latest information to .info file

void closeRelation(Reln r)
{
	// make sure updated global data is put in info
	// Naughty: assumes Count and Offset are the same size
	if (r->mode == 'w') {
		fseek(r->info, 0, SEEK_SET);
		// write out core relation info (#attr,#pages,d,sp)
		int n = fwrite(r, sizeof(Count), 5, r->info);
		assert(n == 5);
		// write out choice vector
		n = fwrite(r->cv, sizeof(ChVecItem), MAXCHVEC, r->info);
		assert(n == MAXCHVEC);
	}
	fclose(r->info);
	fclose(r->data);
	fclose(r->ovflow);
	free(r);
}

// insert a new tuple into a relation
// returns index of bucket where inserted
// - index always refers to a primary data page
// - the actual insertion page may be either a data page or an overflow page
// returns NO_PAGE if insert fails completely
// TODO: include splitting and file expansion

PageID addToRelation(Reln r, Tuple t)
{
	PageID ret = NO_PAGE;
	Bits h, p;
	// char buf[MAXBITS+1];
	h = tupleHash(r,t);
	if (r->depth == 0)
		p = 1;
	else {
		p = getLower(h, r->depth);
		if (p < r->sp) p = getLower(h, r->depth+1);
	}
	// bitsString(h,buf); printf("hash = %s\n",buf);
	// bitsString(p,buf); printf("page = %s\n",buf);
	Page pg = getPage(r->data,p);
	if (addToPage(pg,t) == OK) {
		putPage(r->data,p,pg);
		r->ntups++;
		ret = p;
		// return p;
	}
	// primary data page full
	else if (pageOvflow(pg) == NO_PAGE) {
		// add first overflow page in chain
		PageID newp = addPage(r->ovflow);
		pageSetOvflow(pg,newp);
		putPage(r->data,p,pg);
		Page newpg = getPage(r->ovflow,newp);
		// can't add to a new page; we have a problem
		if (addToPage(newpg,t) != OK) return NO_PAGE;
		putPage(r->ovflow,newp,newpg);
		r->ntups++;
		ret = p;
		// return p;
	}
	else {
		
		// scan overflow chain until we find space
		// worst case: add new ovflow page at end of chain
		Page ovpg, prevpg = NULL;
		PageID ovp, prevp = NO_PAGE;
		ovp = pageOvflow(pg);

		putPage(r->data,p,pg);
		
		while (ovp != NO_PAGE) {
			ovpg = getPage(r->ovflow, ovp);
			if (addToPage(ovpg,t) != OK) {
				prevp = ovp; prevpg = ovpg;
				ovp = pageOvflow(ovpg);

				// free(prevpg);
				putPage(r->ovflow,prevp,ovpg);
			}
			else {
				//if (prevpg != NULL) free(prevpg);
				putPage(r->ovflow,ovp,ovpg);
				r->ntups++;
				ret = p;
				break;
				// return p;
			}
		}
		if (ovp == NO_PAGE) {
			prevpg = getPage(r->ovflow, prevp);
			// all overflow pages are full; add another to chain
			// at this point, there *must* be a prevpg
			assert(prevpg != NULL);
			// make new ovflow page
			PageID newp = addPage(r->ovflow);
			// insert tuple into new page
			Page newpg = getPage(r->ovflow,newp);
			if (addToPage(newpg,t) != OK) return NO_PAGE;
			putPage(r->ovflow,newp,newpg);
			// link to existing overflow chain
			pageSetOvflow(prevpg,newp);
			putPage(r->ovflow,prevp,prevpg);
			r->ntups++;
			ret = p;
			// return p;
		}

	}
	if (needToSplit(r)) {
		
		
		PageID temp = addPage(r->data);
		printf("split: added: %d\n",temp);
		r->npages++;
		distributeTuples(r);

		

		r->sp++;
		
		if (r->sp == pow(2, r->depth)) {
			printf("depth plus 1\n");

			r->depth++;
			r->sp = 0;
		}
	}
	return ret;
}

void distributeTuples(Reln r) {
	PageID curPageID = r->sp;


	FILE* curFile = r->data;

	Page curPage;


	int i;
	char* data;
	// Tuple tuple;
	Offset tupleOffset;

	Vector tuplesInBucket;
	init(&tuplesInBucket, (size_t)floor(PAGESIZE/10 * r->nattrs), CHAR_TYPE);

	Vector freePageIDs;
	init(&freePageIDs, 2, UINT_TYPE);

	PageID tempPageID;
	
	while (curPageID != NO_PAGE) {
		curPage = getPage(curFile, curPageID);
		data = pageData(curPage);
		tupleOffset = 0;

		for (i = 0; i < pageNTuples(curPage); i++) {
			Tuple tuple = (Tuple)calloc(strlen(data + tupleOffset) + 1, sizeof(char));
			tupleOffset += sprintf(tuple, "%s",data + tupleOffset) + 1;
			push(&tuplesInBucket, tuple);
			free(tuple);
		}

		tempPageID = pageOvflow(curPage);
		pageClearPage(curPage);
		if (curPageID != r->sp) {
			push(&freePageIDs, &curPageID);
		}

		putPage(curFile, curPageID,curPage);

		curFile = r->ovflow;
		curPageID = tempPageID;

		// free(curPage);
	}


	i = 0;
	char* buf ;
	Bits p;
	PageID buddyPageID = r->sp + pow(2, r->depth) ;
	PageID buddyBucket = buddyPageID;
	printf("buddy bucket: %d\n",buddyBucket);
	FILE* buddyFile = r->data;
	curFile = r->data;

	curPageID = r->sp;
	curPage = getPage(curFile, curPageID);
	Page buddyPage = getPage(buddyFile, buddyPageID);

	PageID nextPageID = 0;

	while (i < nextFreeSpot(tuplesInBucket)) {
		buf = (char*)get(tuplesInBucket, i);
		p = getLower(tupleHash(r, buf) , r->depth + 1);

		// printf("allocating with depth: %d  ", r->depth + 1);

		if (p == buddyBucket) {
			// printf("to buddy: %d\n", p);
			if (addToPage(buddyPage, buf) == -1) {

				void* nullCheckPtr = pop(freePageIDs);
				if (!nullCheckPtr) { //reuse a page, or add one if you can't
					// printf("adding page\n");
					nextPageID = addPage(r->ovflow);
				}
				else {
					// printf("**using page from freelist %d\n", nextPageID);
					nextPageID = *(PageID*)nullCheckPtr;
				}

				pageSetOvflow(buddyPage, nextPageID);

				putPage(buddyFile, buddyPageID, buddyPage);
				buddyFile = r->ovflow;

				buddyPageID = nextPageID;
					
				buddyPage = getPage(buddyFile, buddyPageID);
			}
		}
		else {
			// printf("to current: %d\n", p);
			

			if (addToPage(curPage, buf) == -1) {
				void* nullCheckPtr = pop(freePageIDs);
				if (!nullCheckPtr) { //reuse a page, or add one if you can't
					// printf("adding page\n");
					nextPageID = addPage(r->ovflow);
				}
				else {
					// printf("**using page from freelist %d\n", nextPageID);
					nextPageID = *(PageID*)nullCheckPtr;
				}

				pageSetOvflow(curPage, nextPageID);

				putPage(curFile, curPageID, curPage);
				curFile = r->ovflow;

				curPageID = nextPageID;
					
				curPage = getPage(curFile, curPageID);
			}
		}

		// printf("%d: %s\n", i, buf);
		i+= strlen(buf) + 1;
	}

	putPage(buddyFile, buddyPageID, buddyPage);
	putPage(curFile, curPageID, curPage);

	// printf("end\n");
	free(freePageIDs);
	free(tuplesInBucket);
}

Status needToSplit(Reln r) {
	if (r->ntups % (int)floor(PAGESIZE/10 * r->nattrs) == 0) {
		return 1;
	}
	return 0;
}

// external interfaces for Reln data

FILE *dataFile(Reln r) { return r->data; }
FILE *ovflowFile(Reln r) { return r->ovflow; }
Count nattrs(Reln r) { return r->nattrs; }
Count npages(Reln r) { return r->npages; }
Count ntuples(Reln r) { return r->ntups; }
Count depth(Reln r)  { return r->depth; }
Count splitp(Reln r) { return r->sp; }
ChVecItem *chvec(Reln r)  { return r->cv; }


// displays info about open Reln

void relationStats(Reln r)
{
	printf("Global Info:\n");
	printf("#attrs:%d  #pages:%d  #tuples:%d  d:%d  sp:%d\n",
	       r->nattrs, r->npages, r->ntups, r->depth, r->sp);
	printf("Choice vector\n");
	printChVec(r->cv);
	printf("Bucket Info:\n");
	printf("%-4s %s\n","#","Info on pages in bucket");
	printf("%-4s %s\n","","(pageID,#tuples,freebytes,ovflow)");
	for (Offset pid = 0; pid < r->npages; pid++) {
		printf("[%2d]  ",pid);
		Page p = getPage(r->data, pid);
		Count ntups = pageNTuples(p);
		Count space = pageFreeSpace(p);
		Offset ovid = pageOvflow(p);
		printf("(d%d,%d,%d,%d)",pid,ntups,space,ovid);
		free(p);
		while (ovid != NO_PAGE) {
			Offset curid = ovid;
			p = getPage(r->ovflow, ovid);
			ntups = pageNTuples(p);
			space = pageFreeSpace(p);
			ovid = pageOvflow(p);
			printf(" -> (ov%d,%d,%d,%d)",curid,ntups,space,ovid);
			free(p);
		}
		putchar('\n');
	}
}
