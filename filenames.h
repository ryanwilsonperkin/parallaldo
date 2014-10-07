/*
 * filenames.h
 *  Functions for dealing with directories and lists of filenames.
 *
 *  author: Ryan Wilson-Perkin
 *  student number: 0719644
 */ 
#ifndef PARALLALDO_FILENAMES_H
#define PARALLALDO_FILENAMES_H

/*
 * get_relative_filename
 *  Concatenates directory and file name using '/' path separator.
 *
 *  dir: directory name
 *  filename: file name
 *
 *  returns: directory name concatenated with file name using '/' path separator
 */
char *get_relative_filename(const char *dir, const char *filename);

/*
 * list_filenames
 *  Read file names from a directory into a list.
 *
 *  dir: the directory to read file names from
 *  filenames: the list to allocate and read file names into
 *
 *  returns: the number of file names read into the list
 */
int list_filenames(const char *dir, char ***filenames);

/*
 * free_filenames
 *  Free space allocated by list_filenames.
 *
 *  n_files: the number of files in the list
 *  filenames: the list allocated by list_filenames
 */
void free_filenames(int n_files, char **filenames);

#endif // PARALLALDO_FILENAMES_H
