#include <stdlib.h>

/*
 * Allocation wrappers, exit process if couldn't allocate.
 */
void *ecalloc(size_t num, size_t size)
{
	void *mem = calloc(num, size);
	if (!mem) {
		exit(1);
	}
	return mem;
}

void *erealloc(void *mem, size_t size)
{
	void *newmem = realloc(mem, size);
	if (!newmem) {
		exit(1);
	}
	return newmem;
}

void efree(void *mem)
{
	if (mem) {
		free(mem);
	}
}