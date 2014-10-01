#ifndef PARALLALDO_PARALLALDO_H
#define PARALLALDO_PARALLALDO_H

typedef struct {
    int x;
    int y;
    int r;
} Position;

typedef struct {
    int width;
    int height;
    char **pixels;
} Parallaldo;

typedef Parallaldo Image;

Parallaldo load_parallaldo(const char *filename);
void free_parallaldo(Parallaldo p);

Image load_image(const char *filename);
void free_image(Image i);

Position find_parallaldo(Parallaldo parallaldo, Image image);

#endif // PARALLALDO_PARALLALDO_H
