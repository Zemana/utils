#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"

/* List of character types */
enum { CHAR = 0, QM = 1, SET = 2, STAR = 3, FINAL = 4 };

typedef struct code {
	uint8_t		type;		/* Type of wild character */
	uint8_t		c;			/* Representation of character */
	uint8_t		*map;		/* Bitmap of set characters */
} code;

/*
 * Return length of the pattern
 */
static int code_count(char *pat)
{
	char c;
	int npat = 0, setf = 0;

	while ((c = *pat++)) {
		switch (c) {
		case '<':
			setf++;
			npat++;
			break;
		case '>':
			setf--;
			break;
		default:
			if (!setf) {
				npat++;
			}
			break;
		}
		if (setf < 0 || setf > 1) {
			return -1;
		}
	}
	if (setf) {
		return -1;
	}
	return npat;
}

/*
 * Parse set and return a bitmap with "1"s for found chars
 */
static uint8_t *parse_set(char *pat)
{
	char c;
	uint8_t *map = ecalloc(1, 256);
	
	while ((c = *pat++) && c != '>') {
		if (c == '|' || c < 0) {
			return NULL;
		}
		if (*pat == '|') {
			int i;
			char from = c, to = *(pat + 1);
			
			if (to == '>') {
				return NULL;
			}
			for (i = from; i <= to; i++) {
				map[i] = 1;
			}
			pat += 2;
		} else {
			map[(int)c] = 1;
		}
	}
	return map;
}

/*
 * Compile pattern string into code list
 */
void *wild_compile(char *pat)
{
	char c;
	code *list = NULL;
	uint8_t *map = NULL;
	int ncode = 0, nlist = 0;
	
	ncode = code_count(pat);
	if (ncode == -1) {
		return NULL;
	}
	ncode += 1;
	
	list = ecalloc(ncode, sizeof(*list));
	
	while ((c = *pat++)) {
		switch (c) {
		case '*':
			list[nlist].type = STAR;
			break;
		case '?':
			list[nlist].type = QM;
			break;
		case '<':
			map = parse_set(pat);
			if (map == NULL) {
				return NULL;
			}
			list[nlist].type = SET;
			list[nlist].map = map;
			pat = strchr(pat, '>') + 1;
			break;
		default:
			list[nlist].type = CHAR;
			list[nlist].c = c;
			break;
		}
		nlist++;
	}
	
	list[nlist++].type = FINAL;
	if (nlist > ncode) {
		return NULL;
	}
	return list;
}

/*
 * Free codes and maps
 */
void wild_free(void *exp)
{
	code *pat = exp;
	while (pat->type != FINAL) {
		if (pat->type == SET) {
			efree(pat->map);
		}
		pat++;
	}
	efree(exp);
}

/* If single char (ch) matches a single pattern(pat) */
#define EQUAL(pat, ch) 										\
	(((pat)->type == CHAR && (pat)->c == (uint8_t)(ch)) ||	\
	((pat)->type == QM) || 									\
	((pat)->type == SET && (pat)->map[(uint8_t)(ch)]))

/*
 * Execute compiled expression to match the string
 */
int wild_execute(void *exp, char *str)
{
	char *last = NULL;
	code *star = NULL;
	code *pat = exp;

	while (*str) {
		switch (pat->type) {
		case CHAR:
			if (pat->c == (uint8_t)*str) {
				str++, pat++;
				continue;
			}
			break;
		case QM:
			str++, pat++;
			continue;
		case SET:
			if (pat->map[(uint8_t)*str]) {
				str++, pat++;
				continue;
			}
			break;
		case STAR:
			do {
				pat++;
			} while (pat->type == STAR);
			if (pat->type == FINAL) {
				return 1;
			}
			star = pat;
			goto FastForward;
		}

		/* mismatch */
		if (star == NULL) {
			return 0;
		}
		pat = star;
		str = last + 1;

FastForward:
		while (!EQUAL(pat, *str)) {
			if (*++str == '\0') {
				return 0;
			}
		}
		last = str;
		str++, pat++;
	}
	while (pat->type == STAR) {
		pat++;
	}
	return (pat->type == FINAL);
}