#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include "utils.h"
#include "trie.h"

#define	MAX_DEPTH		UCHAR_MAX
#define SUPER_DEPTH		10
#define IS_SUPER(x)		(((x)->depth) <= SUPER_DEPTH)

typedef struct node node;

struct node {
	uint8_t		key;		/* Letter of current node */
	void		*value;		/* Value of word node */
	uint8_t		word;		/* Is a word node? */
	uint8_t		depth;		/* Depth level of node */
	uint8_t		nchild;		/* Number of child nodes */
	node		**child;	/* Array of child nodes */
};

/*
 * Free each node and it's children
 */
static void free_nodes(node *root)
{
	uint32_t i, iter = 0;

	if (root == NULL) {
		return;
	}

	iter = IS_SUPER(root) ? 256 : root->nchild;

	for (i = 0; i < iter; i++) {
		free_nodes(root->child[i]);
	}

	efree(root->child);
	efree(root);
}

/*
 * Return child node that has letter=key
 */
static inline node *find_child(node *root, uint8_t key)
{
	uint8_t i;

	if (IS_SUPER(root)) {
		return root->child[key];
	}

	for (i = 0; i < root->nchild; i++) {
		if (root->child[i]->key == key) {
			return root->child[i];
		}
	}
	return NULL;
}

/*
 * Insert string into trie
 */
int trie_insert(void *root, uint8_t *key, uint32_t nkey, void *value)
{
	uint32_t i;
	node *curr = root;

	for (i = 0; i < nkey; i++) {
		node *np = find_child(curr, key[i]);
		if (np == NULL) {
			break;
		}
		curr = np;
	}

	for (; i < nkey; i++) {
		node *new = ecalloc(1, sizeof(*new));

		new->key = key[i];
		new->depth = (curr->depth == MAX_DEPTH) ? MAX_DEPTH : curr->depth + 1;

		if (IS_SUPER(new)) {
			new->child = ecalloc(256, sizeof(node *));
		}

		if (IS_SUPER(curr)) {
			curr->child[key[i]] = new;
		} else {
			uint8_t count = curr->nchild;
			size_t newsize = (count + 1) * sizeof(node *);
			curr->child = erealloc(curr->child, newsize);
			curr->child[count] = new;
		}

		curr->nchild++;
		curr = new;
	}

	/* Word already inserted, duplicate error */
	if (curr->word) {
		return 1;
	}

	curr->word = 1;
	curr->value = value;
	return 0;
}

/*
 * Search a key in the trie
 */
int trie_lookup(void *root, uint8_t *key, uint32_t nkey, void **value)
{
	uint32_t i;
	node *curr = root;

	for (i = 0; i < nkey; i++) {
		node *np = find_child(curr, key[i]);
		if (np == NULL) {
			return 0;
		}
		curr = np;
	}

	if (!curr->word) {
		return 0;
	}
	*value = curr->value;
	return 1;
}

/*
 * Search all possible matches in the trie
 */
void trie_lookup_all(void *root, uint8_t *key, uint32_t nkey, uint32_t nPart, 
					 Offset **OffsetList, lookup_callback match)
{
	uint32_t i, n;

	for (i = 0; i < nkey; i++) {
		node *curr = root;
		for (n = i; n < nkey; n++) {
			node *np = find_child(curr, key[n]);
			if (np == NULL) {
				break;
			}

			if (np->word) {
				match(np->value, i, nPart, OffsetList);
			}
			curr = np;
		}
	}
}

/*
 * Init trie, allocate root node
 */
void *trie_init(void)
{
	node *root = ecalloc(1, sizeof(*root));
	root->child = ecalloc(256, sizeof(node *));
	return root;
}

/*
 * Free trie
 */
void trie_destroy(void *root)
{
	free_nodes(root);
}