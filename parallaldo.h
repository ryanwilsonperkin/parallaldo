#ifndef PARALLALDO_PARALLALDO_H
#define PARALLALDO_PARALLALDO_H

/*
 * Position
 *  A point and orientation of a Parallaldo in an Image.
 *
 *  x: the horizontal index of the Image that coincides with the left edge of the Parallaldo (begins at 1)
 *  y: the vertical index of the Image that coincides with the top edge of the Parallaldo (begins at 1)
 *  r: the rotation (in degrees) of the Parallaldo found within the Image (one of 0, 90, 180, 270)
 */
typedef struct {
    int x;
    int y;
    int r;
} Position;

/*
 * Parallaldo
 *  A representation of a two-dimensional binary image of "Parallaldo".
 *  
 *  width: the width (in number of pixels) of the Parallaldo
 *  height: the height (in number of pixels) of the Parallaldo
 *  pixels: a two-dimensional array of '1's and '0's (each row ends in a null byte)
 */
typedef struct {
    int width;
    int height;
    char **pixels;
} Parallaldo;

/*
 * Image
 *  A representation of a two-dimensional binary image to search for a Parallaldo.
 *  
 *  width: the width (in number of pixels) of the Image
 *  height: the height (in number of pixels) of the Image
 *  pixels: a two-dimensional array of '1's and '0's (each row ends in a null byte)
 *
 *  note: Images and Parallaldos are isomorphic, hence the typedef
 */
typedef Parallaldo Image;

/*
 * load_parallaldo
 *  Read data from file into a new Parallaldo.
 *
 *  filename: the file name to read data from
 *
 *  returns: a new Parallaldo
 */
Parallaldo load_parallaldo(const char *filename);

/*
 * free_parallaldo
 *  Free memory allocated by load_parallaldo.
 *
 *  p: the Parallaldo allocated by load_parallaldo
 */
void free_parallaldo(Parallaldo p);

/*
 * load_image
 *  Read data from file into a new Image.
 *
 *  filename: the file name to read data from
 *
 *  returns: a new Image
 */
Image load_image(const char *filename);

/*
 * free_image
 *  Free memory allocated by load_image.
 *
 *  i: the image allocated by load_parallaldo
 */
void free_image(Image i);

/*
 * find_parallaldo
 *  Find a Parallaldo (if it exists) within an Image.
 *
 *  parallaldo: the Parallaldo to search for
 *  image: the Image to search within
 *
 *  returns: if the Parallaldo can be found within the Image, the Position of occurrence
 *           else, the Position will have x, y, r, all set to 0
 */ 
Position find_parallaldo(Parallaldo parallaldo, Image image);

#endif // PARALLALDO_PARALLALDO_H
