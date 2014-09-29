#include <dirent.h>
#include <pilot.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int listFilenames(const char *dir, char ***filenames)
{
    DIR *dp;
    struct dirent *ep;
    int i, nFiles, filenameLength;

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

int main(int argc, char *argv[]) {
    int i, nProcs, nParallaldos, nImages;
    char **parallaldos, **images;

    nProcs = PI_Configure(&argc, &argv);
    nParallaldos = listFilenames(argv[1], &parallaldos);
    nImages = listFilenames(argv[2], &images);

    PI_StartAll();

    freeFilenames(nParallaldos, parallaldos);
    freeFilenames(nImages, images);
    PI_StopMain(0);
    return 0;
}
