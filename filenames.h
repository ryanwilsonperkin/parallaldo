#ifndef PARALLALDOS_FILENAMES_H
#define PARALLALDOS_FILENAMES_H

int listFilenames(const char *dir, char ***filenames);
void freeFilenames(int nFiles, char **filenames);

#endif // PARALLALDOS_FILENAMES_H
