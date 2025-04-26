#ifndef MOVIE_H
#define MOVIE_H

#include "ImageList.h"
#include "Image.h"

/* Structure for a Movie */
typedef struct {
    ILIST *Frames; /* Pointer to the frame list */
} Movie;

/* Function Prototypes */

/* Allocate the memory space for the movie and the memory space for the frame list.
   Return the pointer to the movie. */
Movie *CreateMovie(void);

/* Release the memory space for the frame list.
   Release the memory space for the movie. */
void DeleteMovie(Movie *movie);

/* Convert a YUV movie to an RGB movie */
void YUV2RGBMovie(Movie *movie);

/* Convert an RGB movie to a YUV movie */
void RGB2YUVMovie(Movie *movie);

Image *YUV2RGBImage(YUVImage *yuv);
YUVImage *RGB2YUVImage(Image *rgb);


#endif /* MOVIE_H */
