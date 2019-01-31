#ifndef __HASH_H
#define __HASH_H

void *hash_init(void);
void hash_destroy(void *handle);
void hash_insert(void *handle, char *name, void *value);
int hash_lookup(void *handle, char *name, void **value);

#endif