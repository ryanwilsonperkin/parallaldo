#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filenames.h"

// Code adapted from GNU libc manual: https://www.gnu.org/software/libc/manual/html_node/Simple-Directory-Lister.html
int listFilenames(const char *dir, char ***filenames)
{
    DIR *dp;
    struct dirent *ep;
    int i, n_files;
    int dir_length, filename_length;

    dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "error: Could not open directory %s.\n", dir);
        return 0;
    }

    n_files = 0;
    while (ep = readdir(dp)) {
        if (ep->d_type == DT_REG) n_files++;
    }
    *filenames = malloc(n_files * sizeof(char *));
    rewinddir(dp);

    i = 0;
    dir_length = strlen(dir);
    while (ep = readdir(dp)) {
        if (ep->d_type == DT_REG) {
            filename_length = strlen(ep->d_name);
            (*filenames)[i] = malloc((dir_length + filename_length + 2) * sizeof(char));
            (*filenames)[i] = strcpy((*filenames)[i], dir);
            (*filenames)[i] = strcat((*filenames)[i], "/");
            (*filenames)[i] = strcat((*filenames)[i], ep->d_name);
            i++;
        }
    }
    return n_files;
}

void freeFilenames(int n_files, char **filenames)
{
    int i;
    for (i = 0; i < n_files; i++) {
        free(filenames[i]);
    }
    free(filenames);
}
