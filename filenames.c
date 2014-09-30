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
    int i, nFiles;

    dp = opendir(dir);
    if (dp != NULL) {
        nFiles = 0;
        while (ep = readdir(dp)) {
            if (ep->d_type == DT_REG) nFiles++;
        }

        *filenames = malloc(nFiles * sizeof(char *));
        rewinddir(dp);

        i = 0;
        while (ep = readdir(dp)) {
            if (ep->d_type == DT_REG) {
                (*filenames)[i] = malloc((strlen(ep->d_name) * sizeof(char)) + 1);
                (*filenames)[i] = strcpy((*filenames)[i], ep->d_name);
                i++;
            }
        }
        return nFiles;
    } else {
        fprintf(stderr, "error: Could not open directory %s.\n", dir);
        return 0;
    }
}

void freeFilenames(int nFiles, char **filenames)
{
    int i;
    for (i = 0; i < nFiles; i++) {
        free(filenames[i]);
    }
    free(filenames);
}
