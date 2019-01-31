#ifndef __UTILS_H
#define __UTILS_H

void *ecalloc(size_t num, size_t size);
void *erealloc(void *mem, size_t size);
void efree(void *mem);

#endif