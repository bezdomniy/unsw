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
#include "libpq/pqformat.h" /* needed for send/recv functions */

#include <stdio.h>
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
#define ADD_ONE(i) (i + 1)

/*****************************************************************************
 * Typedefs
 *****************************************************************************/
typedef struct Email
{
	unsigned char nameLen;
	unsigned char domainLen;
	char name_domain[];
} Email;

typedef struct StringPair
{
	char *a;
	char *b;
} StringPair;

/*****************************************************************************
 * Prototypes
 *****************************************************************************/
StringPair *getLocalDomainPair(Email *email, bool lowercase);
bool parseEmail(char *email);
char *varlena_p_to_string(struct varlena *varlena_ptr);
size_t djb_hash(char *cp);

/*****************************************************************************
 * Helper functions
 *****************************************************************************/

bool parseEmail(char *email)
{
	const char *emailPattern = "^([a-zA-Z]+[a-zA-Z0-9]*)+([-.][a-zA-Z0-9]+)*@([a-zA-Z]+([-][a-zA-Z0-9]+)*[.])+([a-zA-Z]+([-][a-zA-Z0-9]*)?)$";

	regex_t regex;
	int reti = regcomp(&regex, emailPattern, REG_EXTENDED);

	if (reti)
	{
		fprintf(stderr, "Could not compile regex\n");
		exit(1);
	}

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
		fprintf(stderr, "Regex match failed.\n");
		exit(1);
	}

	regfree(&regex);
	return false;
}

StringPair *getLocalDomainPair(Email *email, bool lowercase)
{
	char *str;
	int str_len = ADD_ONE(email->nameLen) + ADD_ONE(email->domainLen);
	int i;
	StringPair *ret = palloc(sizeof(StringPair) + str_len);

	if (lowercase)
	{
		str = (char *)palloc(str_len);
		for (i = 0; i < str_len; i++)
		{
			str[i] = tolower(email->name_domain[i]);
		}
	}
	else
	{
		str = &email->name_domain[0];
	}

	ret->a = palloc(ADD_ONE(email->nameLen));
	ret->b = palloc(ADD_ONE(email->domainLen));

	memcpy(ret->a, str, ADD_ONE(email->nameLen));
	ret->a[ADD_ONE(email->nameLen)] = '\0';
	memcpy(ret->b, str + ADD_ONE(email->nameLen), ADD_ONE(email->domainLen));
	ret->b[ADD_ONE(email->domainLen)] = '\0';

	return ret;
}

/* D. J. Bernstein hash function from
   https://codereview.stackexchange.com/questions/85556/simple-string-hashing-algorithm-implementation */
size_t djb_hash(char *cp)
{
	size_t hash = 5381;
	while (*cp)
		hash = 33 * hash ^ (unsigned char)*cp++;
	return hash;
}

char *varlena_p_to_string(struct varlena *varlena_ptr)
{
	Email *email = VARLENA_P_TO_EMAIL(varlena_ptr);
	char *ret;

	StringPair *pair = getLocalDomainPair(email, true);

	ret = psprintf("%s@%s", pair->a, pair->b);
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

	char *name = palloc(sizeof(char) * MAX_NAME_LENGTH);
	char *domain = palloc(sizeof(char) * MAX_NAME_LENGTH);
	Email *email;
	struct varlena *result;
	char *buf = palloc0(sizeof(char) * MAX_NAME_LENGTH);
	int i;

	unsigned char nameLen;
	unsigned char domainLen;

	if (!parseEmail(str))
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for email: \"%s\"",
						str)));
	}

	buf = strtok(str, "@");
	strcpy(name, buf);

	buf = strtok(NULL, "@");
	strcpy(domain, buf);

	if (strtok(NULL, "@") != NULL)
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for email: \"%s\"",
						str)));
	}

	if (strlen(name) > MAX_NAME_LENGTH)
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for email. Local part too long. Length: %d",
						strlen(name))));
	}

	if (strlen(domain) > MAX_NAME_LENGTH)
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for email. Domain part too long. Length: %d",
						strlen(domain))));
	}

	// Decrement by 1 to fit in 1 byte
	nameLen = strlen(name) - 1;
	domainLen = strlen(domain) - 1;

	result = (struct varlena *)palloc(ADD_ONE(nameLen) + ADD_ONE(domainLen) + VARHDRSZ);
	SET_VARSIZE(result, sizeof(Email) + sizeof(char) * (ADD_ONE(nameLen) + ADD_ONE(domainLen)) + 4);

	email = (Email *)palloc0(sizeof(Email) + sizeof(char) * (ADD_ONE(nameLen) + ADD_ONE(domainLen)));

	for (i = 0; i < ADD_ONE(nameLen); i++)
	{
		email->name_domain[i] = tolower(*(name + i));
	}

	for (i = 0; i < ADD_ONE(domainLen); i++)
	{
		email->name_domain[ADD_ONE(nameLen) + i] = tolower(*(domain + i));
	}

	email->nameLen = (unsigned char)nameLen;
	email->domainLen = (unsigned char)domainLen;

	memcpy(VARDATA(result), email, sizeof(Email) + ADD_ONE(nameLen) + ADD_ONE(domainLen));

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
	int ret = strcmp(&a->name_domain[0], &b->name_domain[0]);

	StringPair *a_pair = getLocalDomainPair(a, true);
	StringPair *b_pair = getLocalDomainPair(b, true);

	if (ret == 0)
	{
		return 0;
	}

	ret = strcmp(a_pair->b, b_pair->b);

	if (ret != 0)
	{
		return ret;
	}

	return strcmp(a_pair->a, b_pair->a);
}

static int
email_abs_domain_cmp_internal(Email *a, Email *b)
{
	StringPair *a_pair = getLocalDomainPair(a, true);
	StringPair *b_pair = getLocalDomainPair(b, true);

	return strcmp(a_pair->b, b_pair->b);
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
	Email *a = VARLENA_P_TO_EMAIL(PG_GETARG_VARLENA_P(0));
	size_t hash = djb_hash(&a->name_domain[0]);

	PG_RETURN_INT32(hash);
}