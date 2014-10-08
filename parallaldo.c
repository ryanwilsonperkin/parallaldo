/*
 * parallaldo.c
 *  Functions for dealing with Parallaldos, Positions, and Images.
 *
 *  author: Ryan Wilson-Perkin
 *  student number: 0719644
 */ 
#include <stdio.h>
#include <stdlib.h>

#include "filenames.h"
#include "parallaldo.h"

/*
 * load_parallaldo
 *  Try to open dir/filename file and read data into Parallaldo.
 *  First two integers (separated by a single space) are the height and width.
 *  Reads "height" number of additional lines.
 *  Additional lines are width + 1 long (newline terminated) and read into pixels as strings.
 */
Parallaldo load_parallaldo(const char *dir, const char *filename)
{
    int i;
    Parallaldo p;
    char *relative_filename = get_relative_filename(dir, filename);
    FILE *f = fopen(relative_filename, "r");

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
    free(relative_filename);
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
Image load_image(const char *dir, const char *filename)
{
    return (Image)load_parallaldo(dir, filename);
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
 * test_location
 *  Iterates over every pixel in p for given rotation, checking against pixels in i.
 *  Returns true iff every pixel in p matches overlapping pixel in i.
 *  Calculates iterator pattern based on rotation.
 */
bool test_location(Parallaldo p, Image i, int iy, int ix, int rotation)
{
    int py_start, py_end, py_step, px_start, px_end, px_step;
    switch (rotation) {
        case 0:
            py_start = 0, py_end = p.height - 1, py_step = 1;
            px_start = 0, px_end = p.width - 1, px_step = 1;
            break;
        case 90:
            py_start = 0, py_end = p.width - 1, py_step = 1;
            px_start = p.height - 1, px_end = 0, px_step = -1;
            break;
        case 180:
            py_start = p.height - 1, py_end = 0, py_step = -1;
            px_start = p.width - 1, px_end = 0, px_step = -1;
            break;
        case 270:
            py_start = p.width - 1, py_end = 0, py_step = -1;
            px_start = 0, px_end = p.height - 1, px_step = 1;
            break;
        default:
            fprintf(stderr, "error: test_location: invalid rotation %d\n", rotation);
            exit(EXIT_FAILURE);
    }

    for (int y_offset = 0, py = py_start; py != py_end; py += py_step, y_offset++) {
        for (int x_offset = 0, px = px_start; px != px_end; px += px_step, x_offset++) {
            switch (rotation) {
                case 0:
                case 180:
                    if (p.pixels[py][px] != i.pixels[iy + y_offset][ix + x_offset]) return false;
                    break;
                case 90:
                case 270:
                    if (p.pixels[px][py] != i.pixels[iy + y_offset][ix + x_offset]) return false;
                    break;
            }
        }
    }
    return true;
}

/*
 * find_parallaldo
 *  Algorithm not yet defined.
 */
Position find_parallaldo(Parallaldo p, Image i)
{
    for (int iy = 0; iy < i.height - p.height; iy++) {
        for (int ix = 0; ix < i.width - p.width; ix++) {
            if (test_location(p, i, iy, ix, 0)) return (Position) {iy + 1, ix + 1, 0};
            if (test_location(p, i, iy, ix, 180)) return (Position) {iy + p.height, ix + p.width, 180};
        }
    }
    for (int iy = 0; iy < i.height - p.width; iy++) {
        for (int ix = 0; ix < i.width - p.height; ix++) {
            if (test_location(p, i, iy, ix, 90)) return (Position) {iy + 1, ix + p.height, 90};
            if (test_location(p, i, iy, ix, 270)) return (Position) {iy + p.width, ix + 1, 270};
        }
    }
    return (Position){0,0,0};
}
