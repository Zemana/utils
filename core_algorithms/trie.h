#ifndef __TRIE_H
#define __TRIE_H

typedef struct Offset Offset;

struct Offset
{
    int     Off;
    Offset *Next;
};

typedef void (__cdecl *lookup_callback)(void *value, uint32_t pos, 
										uint32_t nPart, Offset **OffsetList);

void *trie_init(void);
void trie_destroy(void *root);
int trie_insert(void *root, uint8_t *key, uint32_t nkey, void *value);
int trie_lookup(void *root, uint8_t *key, uint32_t nkey, void **value);
void trie_lookup_all(void *root, uint8_t *key, uint32_t nkey, uint32_t nPart, 
					 Offset **OffsetList, lookup_callback match);

#endif