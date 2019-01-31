#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"

#define	NHASH	100000	/* Maximum hash value & size of symbol table */

typedef struct Nameval Nameval;

struct Nameval {
	char		*name;
	void		*value;
	Nameval		*next;
};

/*
 * Calculate hash value of string (max hash = NHASH).
 */
static uint32_t hash(const char *s)
{
	uint32_t hashval = 0;

	for (hashval = 0; *s != '\0'; s++) {
		hashval = (*s + 37 * hashval);
	}
	return hashval % NHASH;
}

/*
 * Lookup and return symbol, optionally create new one if not found. 
 */
static Nameval *lookup(Nameval **symtab, char *name, int create, void *value)
{
	uint32_t h;
	Nameval *sym = NULL;

	h = hash(name);

	for (sym = symtab[h]; sym != NULL; sym = sym->next) {
		if (strcmp(name, sym->name) == 0) {
			return sym;
		}
	}

	if (create) {
		sym = ecalloc(1, sizeof(*sym));
		sym->name = ecalloc(1, strlen(name) + 1);
		strcpy(sym->name, name);
		sym->value = value;
		sym->next = symtab[h];
		symtab[h] = sym;
	}

	return sym;
}

/*
 * Lookup symbol, return 1 if found, 0 if not found.
 */
int hash_lookup(void *handle, char *name, void **value)
{
	Nameval *nv = lookup(handle, name, 0, NULL);
	if (nv == NULL) {
		return 0;
	}
	*value = nv->value;
	return 1;
}

/*
 * Add a new symbol with value.
 */
void hash_insert(void *handle, char *name, void *value)
{
	lookup(handle, name, 1, value);
}

/*
 * Create a new symbol table, and return it's pointer. 
 */
void *hash_init(void)
{
	Nameval **symtab = ecalloc(NHASH, sizeof(*symtab));
	return symtab;
}

/*
 * Free given symbol table.
 */
void hash_destroy(void *handle)
{
	int i;
	Nameval *sym = NULL;
	Nameval **symtab = handle;
	
	for (i = 0; i < NHASH; i++) {
		for (sym = symtab[i]; sym != NULL; ) {
			Nameval *next = sym->next;
			efree(sym->name);
			efree(sym);
			sym = next;
		}
	}
	efree(symtab);
}