/*
 * selfext.c -- self-extracting archive runtime
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

#include <errno.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "io.h"
#include "selfext_entry.h"
#include "uar.h"

static struct option const long_options[] = {
    { "help",    no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'V' },
    { "verbose", no_argument, NULL, 'v' },
    { "create",  no_argument, NULL, 'c' },
    { NULL,      0,           NULL, 0   }
};

static bool flag_create_destination = false;
static bool flag_verbose_extract = false;

static bool __attribute__ ((unused))
extract_archive_callback (struct uar_archive *uar,
                          struct uar_file *file __attribute__ ((unused)),
                          const char *uar_name __attribute__ ((unused)),
                          const char *fs_name, enum uar_error_level level,
                          const char *message)
{
    if (level == UAR_ELEVEL_NONE)
        {
            if (flag_verbose_extract)
                fprintf (stdout, "[x] - %s\n", fs_name);
        }
    else if (level == UAR_ELEVEL_WARNING)
        perr ("warning: %s: %s\n", fs_name,
              message != NULL ? message : uar_strerror (uar));
    else if (level == UAR_ELEVEL_ERROR)
        perr ("error: %s: %s\n", fs_name,
              message != NULL ? message : uar_strerror (uar));

    return true;
}

/* Since in musl we don't have fmemopen(), and open_memstream can not
   be used to read data, we have to create a temporary file to store
   the archive data to continue.
   This function creates a temporary file and writes the archive data
   to it. */
static FILE *__attribute__ ((__nonnull__ ((1))))
create_data_stream (uint8_t *data, size_t size)
{
    FILE *stream = tmpfile ();
    size_t written = 0;

    if (stream == NULL)
        {
            perr ("Failed to create temporary file: %s\n", strerror (errno));
            exit (EXIT_FAILURE);
        }

    while (written < size)
        {
            size_t n = fwrite (data + written, 1, size - written, stream);

            if (n == 0)
                {
                    perr ("Failed to write to temporary file: %s\n",
                          strerror (errno));
                    exit (EXIT_FAILURE);
                }

            written += n;
        }

    fflush (stream);
    rewind (stream);
    return stream;
}

static void
self_extract (void)
{
    uint8_t *data = uar_get_data_start ();
    size_t size = __uar_data_size;

    if (size == 0 || data == NULL)
        {
            perr ("No archive data found!\n");
            perr ("This archive may be corrupted.\n");
            exit (EXIT_FAILURE);
        }

    FILE *stream = create_data_stream (data, size);
    struct uar_archive *uar = uar_stream_open_file (stream, size);

    if (uar == NULL)
        {
            perr ("Failed to open archive: %s\n",
                  uar_get_ecode (uar) == UAR_SYSCALL_ERROR
                      ? strerror (errno)
                      : uar_strerror (uar));
            exit (EXIT_FAILURE);
        }

    uar_set_extract_callback (uar, extract_archive_callback);
    char *cwd = getcwd (NULL, 0);

    if (cwd == NULL)
        {
            perr ("Failed to get current working directory: %s\n",
                  strerror (errno));
            exit (EXIT_FAILURE);
        }

    if (!uar_stream_extract (uar, cwd))
        {
            perr ("Failed to extract archive: %s\n",
                  uar_get_ecode (uar) == UAR_SYSCALL_ERROR
                      ? strerror (errno)
                      : uar_strerror (uar));
            free (cwd);
            exit (EXIT_FAILURE);
        }

    free (cwd);
    uar_close (uar);
    pinfo ("Extraction completed successfully.\n");
}

static _Noreturn void
show_version (void)
{
    printf ("This is an executable UAR archive.\n");
    printf ("libuar version: %s\n", PACKAGE_VERSION);
    exit (EXIT_SUCCESS);
}

static _Noreturn void
usage (void)
{
    printf ("Usage: %s [OPTION]... [DESTINATION]\n", progname);
    printf ("This is an extractable and executable UAR archive."
            "\n\n");
    printf ("Options:\n");
    printf ("  -h, --help     Display this help and exit\n");
    printf ("  -V, --version  Output version information and "
            "exit\n");
    printf ("  -c, --create   Create DESTINATION if it does not "
            "exist\n");
    printf ("  -v, --verbose  Print verbose output\n");
    printf ("\n");
    printf ("Report bugs to: <" PACKAGE_BUGREPORT ">\n");
    printf ("Home page: <" PACKAGE_URL ">\n");
    exit (EXIT_SUCCESS);
}

static void
create_destination (const char *destination)
{
    if (mkdir (destination, 0755) == -1)
        {
            if (errno != EEXIST)
                {
                    perr ("Failed to create directory '%s': %s\n", destination,
                          strerror (errno));
                    exit (EXIT_FAILURE);
                }
        }
}

int
main (int argc, char **argv)
{
    int opt;

    progname = argv[0];

    while ((opt = getopt_long (argc, argv, "hvc", long_options, NULL)) != -1)
        {
            switch (opt)
                {
                case 'h':
                    usage ();
                case 'V':
                    show_version ();
                case 'c':
                    flag_create_destination = true;
                    break;
                case 'v':
                    flag_verbose_extract = true;
                    break;
                default:
                    fprintf (stderr, "Try '%s --help' for more information.\n",
                             progname);
                    exit (EXIT_FAILURE);
                }
        }

    if (optind + 1 < argc)
        {
            fprintf (stderr, "%s: Too many arguments\n", progname);
            fprintf (stderr, "Try '%s --help' for more information.\n",
                     progname);
            exit (EXIT_FAILURE);
        }

    const char *destination = argc > optind ? argv[optind] : NULL;

    if (destination == NULL)
        {
            perr ("No destination specified\n");
            fprintf (stderr,
                     "To extract the contents of this archive to this\n");
            fprintf (stderr, "directory, please, run '%s .'\n", progname);
            fprintf (stderr, "Try '%s --help' for more information.\n",
                     progname);
            exit (EXIT_FAILURE);
        }

    if (flag_create_destination)
        create_destination (destination);

    if (chdir (destination) == -1)
        {
            perr ("Failed to change directory to '%s': %s\n", destination,
                  strerror (errno));
            exit (EXIT_FAILURE);
        }

    self_extract ();
    return 0;
}