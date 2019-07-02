/*
 * src/tutorial/email.c
 * 
 * COMP9315
 * Term 2
 * Assignment 1
 * Ilia Chibaev
 * z3218424
 * 
 * 
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"
#include "fmgr.h"

#include "access/hash.h"

#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <regex.h>

PG_MODULE_MAGIC;

/*****************************************************************************
 * Macros
 *****************************************************************************/
#define MAX_NAME_LENGTH 256
#define VARLENA_P_TO_EMAIL(e) ((Email *)VARDATA(e))

// ADD_ONE in used to increment length values stored in an unsigned char.
// This is needed because the max length of the local and domain fields is
// 1 greater than the maximum value of an unsigned char.
#define ADD_ONE(i) ((unsigned short)(i + 1))

/*****************************************************************************
 * Typedefs
 *****************************************************************************/
typedef struct Email
{
	unsigned char nameLen;
	unsigned char domainLen;
	char name_domain[];
} Email;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/
char *getLocal(Email *email);
char *getDomain(Email *email);

bool parseEmail(char *email);
char *varlena_p_to_string(struct varlena *varlena_ptr);
char *emailToString(Email *email);

/*****************************************************************************
 * Helper functions
 *****************************************************************************/

bool parseEmail(char *email)
{
	const char *emailPattern = "^([a-zA-Z]|([a-zA-Z][a-zA-Z0-9-]*[a-zA-Z0-9]))([.]([a-zA-Z]|([a-zA-Z][a-zA-Z0-9-]*[a-zA-Z0-9])))*@([a-zA-Z]|([a-zA-Z][a-zA-Z0-9-]*[a-zA-Z0-9]))[.]([a-zA-Z]|([a-zA-Z][a-zA-Z0-9-]*[a-zA-Z0-9]))([.]([a-zA-Z]|([a-zA-Z][a-zA-Z0-9-]*[a-zA-Z0-9])))*$";

	regex_t regex;
	int reti = regcomp(&regex, emailPattern, REG_EXTENDED);

	reti = regexec(&regex, email, 0, NULL, 0);

	if (!reti)
	{
		regfree(&regex);
		return true;
	}
	else if (reti == REG_NOMATCH)
	{
		regfree(&regex);
		return false;
	}
	else
	{
		ereport(ERROR,
			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("regex match failed on input: \"%s\"",
						email)));
		exit(1);
	}

	regfree(&regex);
	return false;
}

char *getLocal(Email *email)
{
	char *str = &email->name_domain[0];
	char *ret = palloc(ADD_ONE(email->nameLen) + 1);

	memcpy(ret, str, ADD_ONE(email->nameLen));
	ret[ADD_ONE(email->nameLen)] = '\0';

	return ret;
}

char *getDomain(Email *email)
{
	char *str = &email->name_domain[0];
	char *ret = palloc(ADD_ONE(email->domainLen) + 1);

	memcpy(ret, str + ADD_ONE(email->nameLen), ADD_ONE(email->domainLen));
	ret[ADD_ONE(email->domainLen)] = '\0';

	return ret;
}

char * emailToString(Email *email) {
	char * out = palloc0(sizeof(char) * (ADD_ONE(email->nameLen) + ADD_ONE(email->domainLen) + 2));

	memcpy(out, &email->name_domain[0], ADD_ONE(email->nameLen));
	out[ADD_ONE(email->nameLen)] = '@';
	
	memcpy(out + ADD_ONE(email->nameLen) + 1, &email->name_domain[ADD_ONE(email->nameLen)], ADD_ONE(email->domainLen));
	out[ADD_ONE(email->nameLen) + ADD_ONE(email->domainLen) + 2] = '\0';

	return out;
}

char *varlena_p_to_string(struct varlena *varlena_ptr)
{
	Email *email = VARLENA_P_TO_EMAIL(varlena_ptr);
	char *ret = emailToString(email);

	return ret;
}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(email_in);

Datum
	email_in(PG_FUNCTION_ARGS)
{
	char *str = strdup(PG_GETARG_CSTRING(0));

	Email *email;
	struct varlena *result;
	int i;

	char *at_ptr;
	unsigned short at_pos = 0;

	unsigned char nameLen;
	unsigned char domainLen;

	if (!parseEmail(str))
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for email: \"%s\"",
						str)));
	}

	at_ptr = strchr(str, '@');
	at_pos = (unsigned short)(at_ptr - str);

	if (at_pos > MAX_NAME_LENGTH)
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for email. Local part too long. Length: %d",
						at_pos)));
	}

	if (strlen(str) - (at_pos + 1) > MAX_NAME_LENGTH)
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for email. Domain part too long. Length: %d",
						strlen(str) - (at_pos + 1))));
	}

	// Decrement by 1 to fit in 1 byte
	nameLen = at_pos - 1;
	domainLen = strlen(str) - (at_pos + 1) - 1;

	result = (struct varlena *)palloc0(sizeof(Email) + sizeof(char) * (ADD_ONE(nameLen) + ADD_ONE(domainLen)) + 1 + VARHDRSZ);
	SET_VARSIZE(result, sizeof(Email) + sizeof(char) * (ADD_ONE(nameLen) + ADD_ONE(domainLen)) + 4 + 1);

	email = (Email *)palloc0(sizeof(Email) + sizeof(char) * (ADD_ONE(nameLen) + ADD_ONE(domainLen)) + 1);

	for (i = 0; i < ADD_ONE(nameLen); i++)
	{
		email->name_domain[i] = tolower(*(str + i));
	}

	for (i = 0; i < ADD_ONE(domainLen); i++)
	{
		email->name_domain[ADD_ONE(nameLen) + i] = tolower(*(str + ADD_ONE(nameLen) + 1 + i));
	}
	email->name_domain[ADD_ONE(nameLen) + ADD_ONE(domainLen)] = '\0';

	free(str);

	email->nameLen = nameLen;
	email->domainLen = domainLen;

	memcpy(VARDATA(result), email, sizeof(Email) + sizeof(char) * (ADD_ONE(nameLen) + ADD_ONE(domainLen)) + 1);

	pfree(email);

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(email_out);

Datum
	email_out(PG_FUNCTION_ARGS)
{
	char *out = varlena_p_to_string(PG_GETARG_VARLENA_P(0));
	PG_RETURN_CSTRING(out);
}

/*****************************************************************************
Comparator helper functions
 *****************************************************************************/

static int
email_abs_cmp_internal(Email *a, Email *b)
{
	char *a_domain = getDomain(a);
	char *b_domain = getDomain(b);
	char *a_local;
	char *b_local;

	int ret = strcmp(a_domain, b_domain);

	pfree(a_domain);
	pfree(b_domain);

	if (ret != 0)
	{
		return ret;
	}

	a_local = getLocal(a);
	b_local = getLocal(b);

	ret = strcmp(a_local, b_local);

	pfree(a_local);
	pfree(b_local);

	return ret;
}

static int
email_abs_domain_cmp_internal(Email *a, Email *b)
{
	char *a_domain = getDomain(a);
	char *b_domain = getDomain(b);

	return strcmp(a_domain, b_domain);
}

/*****************************************************************************
Comparator functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(email_abs_lt);

Datum
	email_abs_lt(PG_FUNCTION_ARGS)
{
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) < 0);
}

PG_FUNCTION_INFO_V1(email_abs_le);

Datum
	email_abs_le(PG_FUNCTION_ARGS)
{
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) <= 0);
}

PG_FUNCTION_INFO_V1(email_abs_eq);

Datum
	email_abs_eq(PG_FUNCTION_ARGS)
{
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) == 0);
}

PG_FUNCTION_INFO_V1(email_abs_ge);

Datum
	email_abs_ge(PG_FUNCTION_ARGS)
{
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) >= 0);
}

PG_FUNCTION_INFO_V1(email_abs_gt);

Datum
	email_abs_gt(PG_FUNCTION_ARGS)
{
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) > 0);
}

PG_FUNCTION_INFO_V1(email_abs_neq);

Datum
	email_abs_neq(PG_FUNCTION_ARGS)
{
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_cmp_internal(a, b) != 0);
}

PG_FUNCTION_INFO_V1(email_abs_domain_eq);

Datum
	email_abs_domain_eq(PG_FUNCTION_ARGS)
{
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_domain_cmp_internal(a, b) == 0);
}

PG_FUNCTION_INFO_V1(email_abs_domain_neq);

Datum
	email_abs_domain_neq(PG_FUNCTION_ARGS)
{
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_BOOL(email_abs_domain_cmp_internal(a, b) != 0);
}

PG_FUNCTION_INFO_V1(email_abs_cmp);

Datum
	email_abs_cmp(PG_FUNCTION_ARGS)
{
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	Email *b = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(1));

	PG_RETURN_INT32(email_abs_cmp_internal(a, b));
}

PG_FUNCTION_INFO_V1(email_abs_hash);

Datum
	email_abs_hash(PG_FUNCTION_ARGS)
{
	struct varlena *varlena_ptr = PG_GETARG_VARLENA_PP(0);
	Datum hash = hash_any((unsigned char *) VARDATA_ANY(varlena_ptr), VARSIZE_ANY_EXHDR(varlena_ptr));;
	PG_FREE_IF_COPY(varlena_ptr, 0);
	PG_RETURN_DATUM(hash);
}
