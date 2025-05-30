#ifndef FILEIO_H
#define FILEIO_H

#include "Image.h"

/* Read an image from a file.                                        */
/* The size of the image needs to be pre-set.                        */
/* The memory space of the image will be allocated in this function. */
/* Return values:                                                    */
/* NULL: fail to load or create an image                             */
/* image: load or create an image successfully                       */
Image *LoadImage(const char *fname);

/* Save a processed image.             */
/* Return values:                      */
/* 0: successfully saved the image     */
/* 1: Cannot open the file for writing */
/* 2: File error while writing to file */
int SaveImage(const char *fname, const Image *image);

#endif
