/*
 * io.c -- I/O helper functions
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

#include "io.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char *progname = NULL;

void
pinfo (char const *format, ...)
{
    va_list args;
    va_start (args, format);
    fprintf (stdout, "%s: ", progname);
    vfprintf (stdout, format, args);
    va_end (args);
}

void
perr (char const *format, ...)
{
    va_list args;
    va_start (args, format);
    fprintf (stderr, "%s: ", progname);
    vfprintf (stderr, format, args);
    va_end (args);
}