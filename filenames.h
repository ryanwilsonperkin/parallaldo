#ifndef PARALLALDO_FILENAMES_H
#define PARALLALDO_FILENAMES_H

int listFilenames(const char *dir, char ***filenames);
void freeFilenames(int nFiles, char **filenames);

#endif // PARALLALDO_FILENAMES_H
