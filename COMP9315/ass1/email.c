/*
 * src/tutorial/email.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */

#include "stdio.h"

PG_MODULE_MAGIC;

typedef struct Email
{	
	unsigned char nameLen;
	unsigned char domainLen; // change to unsigned char
	char name_domain[];
    // unsigned    logged_in;
    // unsigned    logged_out;
}			Email;


/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(email_in);

Datum
email_in(PG_FUNCTION_ARGS)
{
	char *str = strdup(PG_GETARG_CSTRING(0));
	char *name = palloc(sizeof(char) * 255);
	char  *domain = palloc(sizeof(char) * 255);
	// Email *result; 
	struct varlena *result; 
	char *buf = palloc(sizeof(char) * 255);

	unsigned char nameLen;
	unsigned char domainLen;

    buf = strtok(str, "@");
    strcpy(name, buf);

    buf = strtok(NULL, "@");
    strcpy(domain, buf);


    if (strtok(NULL, "@") != NULL) {
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for email: \"%s\"",
						str)));
    }

	nameLen = strlen(name) ;
	domainLen = strlen(domain);

	result = (struct varlena *) palloc(nameLen + domainLen + 1 + VARHDRSZ);
	SET_VARSIZE(result, sizeof(Email) + sizeof(char) * (nameLen + domainLen + 1) + 4);

	Email *email = (Email *) palloc(sizeof(Email) + sizeof(char) * (nameLen + domainLen + 1));

	int i;
	for (i = 0; i < nameLen; i++) {
		email->name_domain[i] = *(name + i);
	}

	for (i = 0; i < domainLen + 1; i++) {
		email->name_domain[nameLen + i] = *(domain + i);
	}

	email->nameLen = (unsigned char) nameLen;
	email->domainLen = (unsigned char) domainLen;

				ereport(WARNING,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("test: \"%s\"",
						email->name_domain)));

	memcpy(VARDATA(result), email, sizeof(Email) + nameLen + domainLen + 1);
	
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(email_out);

Datum
email_out(PG_FUNCTION_ARGS)
{
	// Email    *email = (Email *) PG_GETARG_VARLENA_P(0);
	struct varlena *result = (struct varlena *) PG_GETARG_VARLENA_P(0);
				// 	ereport(WARNING,
				// (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				//  errmsg("test: \"%d\"",
				// 		VARSIZE(result))));
	// struct varlena *result = (struct varlena *) PG_GETARG_POINTER(0);
	// Email *email = (Email *) palloc(sizeof(Email) + VARSIZE(result) - VARHDRSZ);
	Email *email = (Email*)VARDATA(result);

	// 				ereport(WARNING,
	// 			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
	// 			 errmsg("test: \"%d\"",
	// 					email->size)));
	// // memcpy(email, VARDATA(result), VARSIZE(result) - VARHDRSZ);
	
	// 			ereport(WARNING,
	// 			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
	// 			 errmsg("test: \"%s\"",
	// 					email->name_domain)));
	// size_t nameLen = VARSIZE(email->name) - VARHDRSZ;
	// size_t domainLen = VARSIZE(email->domain) - VARHDRSZ;

	char *out = palloc(email->nameLen + email->domainLen + 2 * sizeof(char));

	snprintf(out, email->nameLen+1, "%s", email->name_domain);
	strcat(out, "@");
	strcat(out, &email->name_domain[email->nameLen]);
	//snprintf(out + email->sepPos, email->size - email->sepPos, "%s", &(email->name_domain[email->sepPos]));

	PG_RETURN_CSTRING(out);
}

/*****************************************************************************
 * Operator class for defining B-tree index
 *
 * It's essential that the comparison operators and support function for a
 * B-tree index opclass always agree on the relative ordering of any two
 * data values.  Experience has shown that it's depressingly easy to write
 * unintentionally inconsistent functions.  One way to reduce the odds of
 * making a mistake is to make all the functions simple wrappers around
 * an internal three-way-comparison function, as we do here.
 *****************************************************************************/

// #define Mag(c)	((c)->x*(c)->x + (c)->y*(c)->y)

// static int
// complex_abs_cmp_internal(Complex * a, Complex * b)
// {
// 	double		amag = Mag(a),
// 				bmag = Mag(b);

// 	if (amag < bmag)
// 		return -1;
// 	if (amag > bmag)
// 		return 1;
// 	return 0;
// }


// PG_FUNCTION_INFO_V1(complex_abs_lt);

// Datum
// complex_abs_lt(PG_FUNCTION_ARGS)
// {
// 	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
// 	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

// 	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) < 0);
// }

// PG_FUNCTION_INFO_V1(complex_abs_le);

// Datum
// complex_abs_le(PG_FUNCTION_ARGS)
// {
// 	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
// 	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

// 	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) <= 0);
// }

// PG_FUNCTION_INFO_V1(complex_abs_eq);

// Datum
// complex_abs_eq(PG_FUNCTION_ARGS)
// {
// 	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
// 	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

// 	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) == 0);
// }

// PG_FUNCTION_INFO_V1(complex_abs_ge);

// Datum
// complex_abs_ge(PG_FUNCTION_ARGS)
// {
// 	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
// 	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

// 	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) >= 0);
// }

// PG_FUNCTION_INFO_V1(complex_abs_gt);

// Datum
// complex_abs_gt(PG_FUNCTION_ARGS)
// {
// 	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
// 	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

// 	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) > 0);
// }

// PG_FUNCTION_INFO_V1(complex_abs_cmp);

// Datum
// complex_abs_cmp(PG_FUNCTION_ARGS)
// {
// 	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
// 	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

// 	PG_RETURN_INT32(complex_abs_cmp_internal(a, b));
// }

