#include <stdio.h>
#include <stdlib.h>

#include "parallaldo.h"

Parallaldo load_parallaldo(const char *filename)
{
    int i;
    Parallaldo p;
    FILE *f = fopen(filename, "r");

    if (f) {
        fscanf(f, "%d %d", &(p.height), &(p.width));
        p.pixels = malloc(p.height * sizeof(char *));
        for (i = 0; i < p.height; i++) {
            p.pixels[i] = malloc((p.width + 1) * sizeof(char));
            fscanf(f, "%s ", p.pixels[i]);
            p.pixels[i][p.width] = '\0';
        }
    }
    return p;
}

void free_parallaldo(Parallaldo p)
{
    int i;
    for (i = 0; i < p.height; i++) {
        free((p.pixels)[i]);
    }
    free(p.pixels);
}


Image load_image(const char *filename)
{
    return (Image)load_parallaldo(filename);
}

void free_image(Image i)
{
    free_parallaldo((Parallaldo)i);
}
