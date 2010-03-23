/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include "string.h"

char *strcat(char * s1, const char * s2)
{
	char *s = s1;

	while (*s++);
	--s;
	while ((*s++ = *s2++) != 0);

	return s1;
}
