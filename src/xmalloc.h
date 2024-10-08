/*
 * xmalloc.h - definitions of memory allocation wrappers
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

#ifndef UAR_XMALLOC_H
#define UAR_XMALLOC_H

#include <stddef.h>

/* Allocate memory and abort process on error. */
void *xmalloc (size_t size);

/* Allocate 'n' zeroed memory block of 'size' and abort process on error. */
void *xcalloc (size_t nmemb, size_t size);

/* Reallocate memory and abort process on error. */
void *xrealloc (void *ptr, size_t size);

#endif /* UAR_XMALLOC_H */