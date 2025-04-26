#include <stdlib.h>
#include <assert.h>
#include "Movie.h"
#include "Image.h" 

/* Allocate the memory space for the movie and the memory space */
/* for the frame list. Return the pointer to the movie. */
Movie *CreateMovie(void)
{
        Movie *newMovie = (Movie *)malloc(sizeof(Movie)); // allocate memory for movie structure
        if (!newMovie) {
                return NULL; // catch mem allocation failure
        }

        newMovie->Frames = CreateImageList(); // create frame list
        if (!newMovie->Frames) {
                free(newMovie); // catch allocation failure
                return NULL;
        }

        return newMovie; // return movie
}

/* Release the memory space for the frame list. */
/* Release the memory space for the movie. */
void DeleteMovie(Movie *movie)
{
        if (!movie) { // check if movie pointer is NULL
                return;
        }

        DeleteImageList(movie->Frames); // free memory used by frames
        free(movie); // free actual movie structure
}


/* Convert a YUV movie to a RGB movie */
void YUV2RGBMovie(Movie *movie)
{
        IENTRY *entry = movie->Frames->First; // start with first frame in movie
        while (entry != NULL) {
                YUVImage *yuvImag = (YUVImage *)entry->image; // cast image into YUV format
                unsigned int w = yuvImag->W;
                unsigned int h = yuvImag->H;
                Image *rgbImag = CreateImage(w, h); // make new rgb iamge w/same dimensions
                
		for (unsigned int y = 0; y < h; y++) {
                        for (unsigned int x = 0; x < w; x++) { // loop through each pixel
                                int C = (int)GetPixelY(yuvImag, x, y) - 16; // grab YUV components for each pixel
                                int D = (int)GetPixelU(yuvImag, x, y) - 128;
                                int E = (int)GetPixelV(yuvImag, x, y) - 128;

                                int R = (298 * C + 409 * E + 128) >> 8; // YUV to rgb
                                int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
                                int B = (298 * C + 516 * D + 128) >> 8;

                                // set range to [0, 255]
                                R = (R < 0) ? 0 : (R > 255 ? 255 : R);
                                G = (G < 0) ? 0 : (G > 255 ? 255 : G);
                                B = (B < 0) ? 0 : (B > 255 ? 255 : B);

                                // set rgb pixel in new image
                                SetPixelR(rgbImag, x, y, (unsigned char)R);
                                SetPixelG(rgbImag, x, y, (unsigned char)G);
                                SetPixelB(rgbImag, x, y, (unsigned char)B);
                        }
                }

                // delete old YUV image
                DeleteYUVImage(yuvImag);

                // implement new rgb for image
                entry->image = rgbImag;

                // next frame
                entry = entry->Next;
        }
}


/* Convert a RGB movie to a YUV movie */
void RGB2YUVMovie(Movie *movie)
{
        IENTRY *entry = movie->Frames->First; // start with first frame in movie
        while (entry != NULL) {
                Image *rgbImag = (Image *)entry->image; // grab current rgb image from frame
                unsigned int w = rgbImag->W;
                unsigned int h = rgbImag->H;
                YUVImage *yuvImag = CreateYUVImage(w, h); // allocate new YUV image w/same dimensions

                for (unsigned int y = 0; y < h; y++) {
                        for (unsigned int x = 0; x < w; x++) { // iterate through each pixel
                                int R = (int)GetPixelR(rgbImag, x, y); // grab the rgb values
                                int G = (int)GetPixelG(rgbImag, x, y);
                                int B = (int)GetPixelB(rgbImag, x, y);

                                // calc rgb to YUV
                                int Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
                                int U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
                                int V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

                                // set range to [0, 255]
                                Y = (Y < 0) ? 0 : (Y > 255 ? 255 : Y);
                                U = (U < 0) ? 0 : (U > 255 ? 255 : U);
                                V = (V < 0) ? 0 : (V > 255 ? 255 : V);

                                // set converted YUV values in new image
                                SetPixelY(yuvImag, x, y, (unsigned char)Y);
                                SetPixelU(yuvImag, x, y, (unsigned char)U);
                                SetPixelV(yuvImag, x, y, (unsigned char)V);
                        }
                }

                // free old image
                DeleteImage(rgbImag);

                // replace frame's image with new YUV image
                entry->image = yuvImag;

                // next frame
                entry = entry->Next;
        }
}

