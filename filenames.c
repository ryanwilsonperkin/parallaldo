// DT_REG is a part of the GNU/BSD extension.
#define _GNU_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filenames.h"

/*
 * get_relative_filename
 *  Calculate and allocate needed space for a new string combining dir and filename.
 *  Concatenate dir, '/', and filename, and return the resulting string.
 */
char *get_relative_filename(const char *dir, const char *filename)
{
    int dir_length = strlen(dir);
    int filename_length = strlen(filename);

    char *relative_filename = malloc((dir_length + filename_length + 2) * sizeof(char));
    if (relative_filename == NULL) {
        fprintf(stderr, "error: get_relative_filename: can't initialize relative_filename, out of memory.\n");
        exit(EXIT_FAILURE);
    }
    relative_filename = strcpy(relative_filename, dir);
    relative_filename = strcat(relative_filename, "/");
    relative_filename = strcat(relative_filename, filename);
    return relative_filename;
}

/*
 * list_filenames
 *  Count number of files in dir, allocate same number of strings in filenames list.
 *  Read name of each file in dir and add filename string to list.
 *  note: adapted from GNU libc manual: https://www.gnu.org/software/libc/manual/html_node/Simple-Directory-Lister.html
 */
int list_filenames(const char *dir, char ***filenames)
{
    DIR *dp;
    struct dirent *ep;
    int i, n_files;

    dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "error: Could not open directory %s.\n", dir);
        return 0;
    }

    n_files = 0;
    while (ep = readdir(dp)) {
        if (ep->d_type == DT_REG) {
            n_files++;
        }
    }

    *filenames = malloc(n_files * sizeof(char *));
    if (filenames == NULL) {
        fprintf(stderr, "error: list_filenames: can't initialize filenames, out of memory.\n");
        exit(EXIT_FAILURE);
    }
    rewinddir(dp);

    i = 0;
    while (ep = readdir(dp)) {
        if (ep->d_type == DT_REG) {
            (*filenames)[i] = malloc((strlen(ep->d_name) + 1) * sizeof(char));
            if ((*filenames)[i] == NULL) {
                fprintf(stderr, "error: list_filenames: can't initialize (*filenames)[%d], out of memory.\n", i);
                exit(EXIT_FAILURE);
            }
            (*filenames)[i] = strcpy((*filenames)[i], ep->d_name);
            i++;
        }
    }

    closedir(dp);
    return n_files;
}

/* 
 * free_filename
 *  Free each string in filenames list.
 *  Free filenames list.
 */
void free_filenames(int n_files, char **filenames)
{
    for (int i = 0; i < n_files; i++) {
        free(filenames[i]);
    }
    free(filenames);
}
