#ifndef PARALLALDO_PARALLALDO_H
#define PARALLALDO_PARALLALDO_H

typedef struct Parallaldo {
    int width;
    int height;
    char **pixels;
} Parallaldo;

Parallaldo load_parallaldo(const char *filename);
void free_parallaldo(Parallaldo p);

#endif // PARALLALDO_PARALLALDO_H
