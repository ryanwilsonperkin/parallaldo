#include <stdio.h>
#include <stdlib.h>

#include "parallaldo.h"

/*
 * load_parallaldo
 *  Try to open filename file and read data into Parallaldo.
 *  First two integers (separated by a single space) are the height and width.
 *  Reads "height" number of additional lines.
 *  Additional lines are width + 1 long (newline terminated) and read into pixels as strings.
 */
// TODO: Print error if file couldn't open.
// TODO: Close file.
Parallaldo load_parallaldo(const char *filename)
{
    int i;
    Parallaldo p;
    FILE *f = fopen(filename, "r");

    if (f) {
        fscanf(f, "%d %d", &(p.height), &(p.width));
        p.pixels = malloc(p.height * sizeof(char *));
        if (p.pixels == NULL) {
            fprintf(stderr, "error: load_parallaldo: can't initialize p.pixels, out of memory.\n");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < p.height; i++) {
            p.pixels[i] = malloc((p.width + 1) * sizeof(char));
            if (p.pixels[i] == NULL) {
                fprintf(stderr, "error: load_parallaldo: can't initialize p.pixels[%d], out of memory.\n", i);
                exit(EXIT_FAILURE);
            }
            fscanf(f, "%s ", p.pixels[i]);
            p.pixels[i][p.width] = '\0';
        }
    }
    return p;
}

/*
 * free_parallaldo
 *  Free each string in pixels list.
 *  Free pixels list.
 */
void free_parallaldo(Parallaldo p)
{
    for (int i = 0; i < p.height; i++) {
        free((p.pixels)[i]);
    }
    free(p.pixels);
}

/*
 * load_image
 *  Images are isomorphic to Parallaldos, so defer to load_parallaldo and cast result to Image.
 */
Image load_image(const char *filename)
{
    return (Image)load_parallaldo(filename);
}

/*
 * free_image
 *  Images are isomorphic to Parallaldos, so defer to free_parallaldo and cast parameter to Parallaldo.
 */
void free_image(Image i)
{
    free_parallaldo((Parallaldo)i);
}

/*
 * find_parallaldo
 *  Algorithm not yet defined.
 */
// TODO: Implement algorithm.
// TODO: Default position x,y,r should be 0.
Position find_parallaldo(Parallaldo parallaldo, Image image)
{
    Position p;
    p.x = -1;
    p.y = -1;
    p.r = -1;
    return p;
}
