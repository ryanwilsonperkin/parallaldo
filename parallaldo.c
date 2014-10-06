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
        fclose(f);
    } else {
        fprintf(stderr, "error: load_parallaldo: could not open file %s for read.\n", filename);
        exit(EXIT_FAILURE);
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
// TODO: Improve algorithm.
Position find_parallaldo(Parallaldo parallaldo, Image image)
{
    int found;
    for (int iy = 0; iy < image.height - parallaldo.height; iy++) {
        for (int ix = 0; ix < image.width - parallaldo.width; ix++) {
            found = 1;
            for (int py = 0; py < parallaldo.height; py++) {
                for (int px = 0; px < parallaldo.width; px++) {
                    int y_offset = py, x_offset = px;
                    if (parallaldo.pixels[py][px] != image.pixels[iy + y_offset][ix + x_offset]) found = 0;
                }
            }
            if (found) return (Position){iy + 1, ix + 1, 0};
            found = 1;
            for (int py = parallaldo.height - 1; py >= 0; py--) {
                for (int px = parallaldo.width - 1; px >= 0; px--) {
                    int y_offset = parallaldo.height - 1 - py, x_offset = parallaldo.width - 1 - px;
                    if (parallaldo.pixels[py][px] != image.pixels[iy + y_offset][ix + x_offset]) found = 0;
                }
            }
            if (found) return (Position){iy + parallaldo.height, ix + parallaldo.width, 180};
        }
    }
    for (int iy = 0; iy < image.height - parallaldo.width; iy++) {
        for (int ix = 0; ix < image.width - parallaldo.height; ix++) {
            found = 1;
            for (int py = 0; py < parallaldo.width; py++) {
                for (int px = parallaldo.height - 1; px >= 0; px--) {
                    int y_offset = py, x_offset = parallaldo.height - 1 - px;
                    if (parallaldo.pixels[px][py] != image.pixels[iy + y_offset][ix + x_offset]) found = 0;
                }
            }
            if (found) return (Position){iy + 1, ix + parallaldo.height, 90};
            found = 1;
            for (int py = parallaldo.width - 1; py >= 0; py--) {
                for (int px = 0; px < parallaldo.height; px++) {
                    int y_offset = parallaldo.width - 1 - py, x_offset =  px;
                    if (parallaldo.pixels[px][py] != image.pixels[iy + y_offset][ix + x_offset]) found = 0;
                }
            }
            if (found) return (Position){iy + parallaldo.width, ix + 1, 270};
        }
    }
    return (Position){0,0,0};
}
