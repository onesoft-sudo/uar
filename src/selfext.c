/*
 * selfext.c -- self-extracting archive runtime
 *
 * This file is part of the UAR project.
 * Copyright (C) 2024  OSN, Inc.
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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "uar.h"

extern long long int __uar_data_size;
extern uint8_t *uar_get_data_start (void);
FILE *open_memstream (unsigned char **bufp, size_t *sizep);

static bool
extract_archive_callback (struct uar_archive *uar,
                          struct uar_file *file __attribute__ ((unused)),
                          const char *uar_name __attribute__ ((unused)),
                          const char *fs_name, enum uar_error_level level,
                          const char *message)
{
    if (level == UAR_ELEVEL_NONE)
        {
            fprintf (stdout, "%s\n", fs_name);
        }
    else if (level == UAR_ELEVEL_WARNING)
        fprintf (stderr, "warning: %s: %s\n", fs_name,
                 message != NULL ? message : uar_strerror (uar));
    else if (level == UAR_ELEVEL_ERROR)
        fprintf (stderr, "error: %s: %s\n", fs_name,
                 message != NULL ? message : uar_strerror (uar));

    return true;
}

int
main (void)
{
    uint8_t *data = uar_get_data_start ();
    size_t size = __uar_data_size;
    FILE *stream = tmpfile ();

    if (stream == NULL)
        {
            fprintf (stderr, "tmpfile: %s\n", strerror (errno));
            return 1;
        }

    size_t written = 0;

    while (written < size)
        {
            size_t n = fwrite (data + written, 1, size - written, stream);

            if (n == 0)
                {
                    fprintf (stderr, "fwrite: %s\n", strerror (errno));
                    return 1;
                }

            written += n;
        }

    fflush (stream);
    rewind (stream);

    struct uar_archive *uar = uar_stream_open_file (stream, __uar_data_size);

    if (uar == NULL || uar_has_error (uar))
        {
            fprintf (stderr, "uar_stream_open_file: %s\n", uar_strerror (uar));
            return 1;
        }

    uar_set_extract_callback (uar, &extract_archive_callback);
    chdir ("test");
    char *cwd = getcwd (NULL, 0);

    if (!uar_stream_extract (uar, cwd))
        {
            fprintf (stderr, "failed to extract archive: %s\n",
                     strerror (errno));
            free (cwd);
            return 1;
        }

    free (cwd);
    uar_close (uar);
    return 0;
}