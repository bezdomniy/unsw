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
#include "string.h"
#include "ctype.h"

PG_MODULE_MAGIC;

typedef struct Email
{	
	unsigned char nameLen;
	unsigned char domainLen;
	char name_domain[];
}			Email;

#define VARLENA_P_TO_EMAIL(e) ((Email*)VARDATA(e))

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
	Email *email; 
	struct varlena *result; 
	char *buf = palloc(sizeof(char) * 255);
	int i;

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

	email = (Email *) palloc(sizeof(Email) + sizeof(char) * (nameLen + domainLen + 1));

	
	for (i = 0; i < nameLen; i++) {
		email->name_domain[i] = *(name + i);
	}

	for (i = 0; i < domainLen + 1; i++) {
		email->name_domain[nameLen + i] = *(domain + i);
	}

	email->nameLen = (unsigned char) nameLen;
	email->domainLen = (unsigned char) domainLen;

				// ereport(WARNING,
				// (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				//  errmsg("test: \"%s\"",
				// 		email->name_domain)));

	memcpy(VARDATA(result), email, sizeof(Email) + nameLen + domainLen + 1);
	
	PG_RETURN_POINTER(result);
}

typedef struct StringPair {
    char* a;
    char* b;
} StringPair;

// StringPair* splitString(char* str, int nameLen, int domainLen) {
StringPair* getLocalDomainPair(Email* email, bool lowercase) {
	char * str;
	int str_len = email->nameLen + email->domainLen + 1;
	int i;
	
	if (lowercase) {
		str = (char*) palloc(str_len);
		for (i = 0; i < str_len; i++) {
			str[i] = tolower(email->name_domain[i]);
		}
	}
	else {
		str = &email->name_domain[0];
	}

	StringPair *ret = palloc(sizeof(StringPair) + str_len + 1);;
    
    ret->a = palloc(email->nameLen + 1);
    ret->b = palloc(email->domainLen + 1);

    memcpy(ret->a, str, email->nameLen);
    ret->a[email->nameLen] = '\0';
    memcpy(ret->b, str + email->nameLen, email->domainLen);
	ret->b[email->domainLen] = '\0';

	return ret;
}

PG_FUNCTION_INFO_V1(email_out);

char* varlena_p_to_string(struct varlena *varlena_ptr) {
	Email *email = VARLENA_P_TO_EMAIL(varlena_ptr);
	char *ret;

	StringPair* pair = getLocalDomainPair(email, true);
	//char *ret = palloc(email->nameLen + email->domainLen + 2 * sizeof(char));

	ret = psprintf("%s@%s", pair->a, pair->b);
	return ret;
}

Datum
email_out(PG_FUNCTION_ARGS)
{
	char *out = varlena_p_to_string(PG_GETARG_VARLENA_P(0));
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

static int
email_abs_cmp_internal(Email * a, Email * b)
{
	int ret = strcmp(&a->name_domain[0], &b->name_domain[0]);
	if (ret == 0) {
		return 0;
	}

	StringPair* a_pair = getLocalDomainPair(a, true);
	StringPair* b_pair = getLocalDomainPair(b, true);

	ret = strcmp(a_pair->b, b_pair->b);

	if (ret != 0) {
		return ret;
	}

	return strcmp(a_pair->a, b_pair->a);
}

static int
email_abs_domain_cmp_internal(Email * a, Email * b)
{
	StringPair* a_pair = getLocalDomainPair(a, true);
	StringPair* b_pair = getLocalDomainPair(b, true);

	return strcmp(a_pair->b, b_pair->b) == 0;
}


PG_FUNCTION_INFO_V1(email_abs_lt);

Datum
email_abs_lt(PG_FUNCTION_ARGS)
{
	Email    *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email    *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) < 0);
}

PG_FUNCTION_INFO_V1(email_abs_le);

Datum
email_abs_le(PG_FUNCTION_ARGS)
{
	Email    *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email    *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) <= 0);
}

PG_FUNCTION_INFO_V1(email_abs_eq);

Datum
email_abs_eq(PG_FUNCTION_ARGS)
{
	Email    *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email    *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) == 0);
}

PG_FUNCTION_INFO_V1(email_abs_ge);

Datum
email_abs_ge(PG_FUNCTION_ARGS)
{
	Email    *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email    *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) >= 0);
}

PG_FUNCTION_INFO_V1(email_abs_gt);

Datum
email_abs_gt(PG_FUNCTION_ARGS)
{
	Email    *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email    *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) > 0);
}

PG_FUNCTION_INFO_V1(email_abs_neq);

Datum
email_abs_neq(PG_FUNCTION_ARGS)
{
	Email    *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email    *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) != 0);
}

PG_FUNCTION_INFO_V1(email_abs_domain_eq);

Datum
email_abs_domain_eq(PG_FUNCTION_ARGS)
{
	Email    *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email    *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_domain_cmp_internal(a, b) == 0);
}

PG_FUNCTION_INFO_V1(email_abs_domain_neq);

Datum
email_abs_domain_neq(PG_FUNCTION_ARGS)
{
	Email    *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email    *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_domain_cmp_internal(a, b) != 0);
}

PG_FUNCTION_INFO_V1(email_abs_cmp);

Datum
email_abs_cmp(PG_FUNCTION_ARGS)
{
	Email    *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email    *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_INT32(email_abs_cmp_internal(a, b));
}

