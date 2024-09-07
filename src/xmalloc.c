/*
 * xmalloc.c - memory allocation wrappers
 *
 * This file is part of the UAR project.
 * Copyright (C) 2024  OSN Developers.
 * Author:  Ar Rakin <rakinar2@onesoftnet.eu.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "xmalloc.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *
xmalloc (size_t size)
{
    void *ptr = malloc (size);

    if (ptr == NULL)
        {
            fprintf (stderr, "xmalloc(%zu): failed to allocate memory: %s",
                     size, strerror (errno));
            abort ();
        }

    return ptr;
}

void *
xcalloc (size_t nmemb, size_t size)
{
    void *ptr = calloc (nmemb, size);

    if (ptr == NULL)
        {
            fprintf (stderr, "xcalloc(%zu, %zu): failed to allocate memory: %s",
                     nmemb, size, strerror (errno));
            abort ();
        }

    return ptr;
}

void *
xrealloc (void *ptr, size_t size)
{
    void *new_ptr = realloc (ptr, size);

    if (new_ptr == NULL)
        {
            fprintf (stderr,
                     "xrealloc(%p, %zu): failed to re-allocate memory: %s", ptr,
                     size, strerror (errno));
            abort ();
        }

    return new_ptr;
}