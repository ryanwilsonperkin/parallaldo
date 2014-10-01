// DT_REG is a part of the GNU/BSD extension.
#define _GNU_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filenames.h"

// Combine directory and file name, allocate space.
char *get_relative_filename(const char *dir, const char *filename)
{
    int dir_length = strlen(dir);
    int filename_length = strlen(filename);

    char *relative_filename = malloc((dir_length + filename_length + 2) * sizeof(char));
    relative_filename = strcpy(relative_filename, dir);
    relative_filename = strcat(relative_filename, "/");
    relative_filename = strcat(relative_filename, filename);
    return relative_filename;
}

// Code adapted from GNU libc manual: https://www.gnu.org/software/libc/manual/html_node/Simple-Directory-Lister.html
int listFilenames(const char *dir, char ***filenames)
{
    DIR *dp;
    struct dirent *ep;
    int i, n_files;

    // Try to open directory.
    dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "error: Could not open directory %s.\n", dir);
        return 0;
    }

    // Increment number of files for each DT_REG entity found.
    n_files = 0;
    while (ep = readdir(dp)) {
        if (ep->d_type == DT_REG) {
            n_files++;
        }
    }

    // Allocate a string pointer for each filename.
    *filenames = malloc(n_files * sizeof(char *));

    // Return to first entity in directory listing.
    rewinddir(dp);

    // Allocate filename for each DT_REG entity found.
    i = 0;
    while (ep = readdir(dp)) {
        if (ep->d_type == DT_REG) {
            (*filenames)[i] = get_relative_filename(dir, ep->d_name);
            i++;
        }
    }
    closedir(dp);
    return n_files;
}

// Free space allocated to list of filenames.
void freeFilenames(int n_files, char **filenames)
{
    int i;
    for (i = 0; i < n_files; i++) {
        free(filenames[i]);
    }
    free(filenames);
}
