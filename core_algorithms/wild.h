#ifndef __WILD_H
#define __WILD_H

void *wild_compile(char *pat);
int wild_execute(void *exp, char *str);
void wild_free(void *exp);

#endif