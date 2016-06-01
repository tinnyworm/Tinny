/**
 * @file
 * @author Danfeng Li <danfengli@fairisaac.com>
 * @date
 *
 * @brief
 *
 * $Log:$
 *
 * Copied from http://funknet.net/rfunk/Linux/progs/split.c
 *
 */

#ifndef SPLIT_H
#define SPLIT_H

#include <string.h>
#include <stdlib.h>

#define MAXLEN 256

char **split (const char *delim, const char *string);
int countdelim (const char *delim, const char *string);

#endif /* SPLIT_H */
