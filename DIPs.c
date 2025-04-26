#include <stdlib.h>
#include <assert.h>
#include "DIPs.h"
#include "Constants.h"
#include "string.h"
#include "math.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

/* Black and White */
Image *BlackNWhite(Image *image)
{
        if (!image) {
                return NULL; // make sure both images are valid
        }
        // loop through each pixel
        for (int y = 0; y < ImageHeight(image); y++) {
                for (int x = 0; x < ImageWidth(image); x++) {
                        // average the rgb to get grayscale value
                        int avgTemp = (GetPixelR(image, x, y) + GetPixelG(image, x, y) + GetPixelB(image, x, y)) / 3;

                        // set all channels to the gray value
                        SetPixelR(image, x, y, avgTemp);
                        SetPixelG(image, x, y, avgTemp);
                        SetPixelB(image, x, y, avgTemp);
                }
        }

        return image; // return grayscale image
}

/* Mirror image horizontal  */
Image *HMirror(Image *image) {
	if (!image) {
                return NULL; // make sure both images are valid
        }

        // go through each row
        for (int y = 0; y < ImageHeight(image); y++) {
                // only loop halfway across each row
                for (int x = 0; x < ImageWidth(image) / 2; x++) {
                        // copy left side pixels to the right side (flipped horizontally)
                        SetPixelG(image, ImageWidth(image) - 1 - x, y, GetPixelG(image, x, y));
                        SetPixelB(image, ImageWidth(image) - 1 - x, y, GetPixelB(image, x, y));
                        SetPixelR(image, ImageWidth(image) - 1 - x, y, GetPixelR(image, x, y));
                }
        }

        return image; // return flipped image
}

/* Edge detection */
Image *Edge(Image *image) {
        assert(image);
        Image *tempImage = CreateImage(ImageWidth(image), ImageHeight(image));
        assert(tempImage);
	int x, y;
        // copy image data to temp image
        memcpy(tempImage->R, image->R, ImageWidth(image) * ImageHeight(image) * sizeof(unsigned char));
        memcpy(tempImage->G, image->G, ImageWidth(image) * ImageHeight(image) * sizeof(unsigned char));
        memcpy(tempImage->B, image->B, ImageWidth(image) * ImageHeight(image) * sizeof(unsigned char));

        // loop through interior pixels (skip borders)
        for (int y = 1; y < ImageHeight(image) - 1; y++) {
                for (int x = 1; x < ImageWidth(image) - 1; x++) {
                        int tempR = 0, tempG = 0, tempB = 0;

                        // check for edges based on neighboring pixels
                        for (int m = -1; m <= 1; m++) {
                                for (int n = -1; n <= 1; n++) {
                                        tempR += (GetPixelR(tempImage, x, y) - GetPixelR(tempImage, x + n, y + m));
                                        tempG += (GetPixelG(tempImage, x, y) - GetPixelG(tempImage, x + n, y + m));
                                        tempB += (GetPixelB(tempImage, x, y) - GetPixelB(tempImage, x + n, y + m));
                                }
                        }
                        // ensure rgb is within range
                        SetPixelR(image, x, y, (tempR > 255) ? 255 : (tempR < 0) ? 0 : tempR);
                        SetPixelG(image, x, y, (tempG > 255) ? 255 : (tempG < 0) ? 0 : tempG);
                        SetPixelB(image, x, y, (tempB > 255) ? 255 : (tempB < 0) ? 0 : tempB);
                }
        }

	// set top and bottom edges to black
        for (x = 0; x < ImageWidth(image); x++) {
                y = 0;
                SetPixelR(image, x, y, 0);
                SetPixelG(image, x, y, 0);
                SetPixelB(image, x, y, 0);

                y = ImageHeight(image) - 1;
                SetPixelR(image, x, y, 0);
                SetPixelG(image, x, y, 0);
                SetPixelB(image, x, y, 0);
        }

	// set left and right edges to black
        for (y = 0; y < ImageHeight(image); y++) {
                x = 0;
                SetPixelR(image, x, y, 0);
                SetPixelG(image, x, y, 0);
                SetPixelB(image, x, y, 0);

                x = ImageWidth(image) - 1;
                SetPixelR(image, x, y, 0);
                SetPixelG(image, x, y, 0);
                SetPixelB(image, x, y, 0);
        }


        // remove temp image
        DeleteImage(tempImage);
        tempImage = NULL;

        return image; // return new image
}

/* Watermark */
Image *Watermark(Image *image, const Image *watermark,
                 unsigned int topLeftX, unsigned int topLeftY)
{
        if (!image || !watermark) {
                return NULL; // make sure both images are valid
        }

        unsigned int imagW = image->W;
        unsigned int imagH = image->H;
        unsigned int wmW = watermark->W;
        unsigned int wmH = watermark->H;

        // check if watermark fits in the image
        if (topLeftX + wmW > imagW || topLeftY + wmH > imagH) {
                return NULL; // watermark would go out of bounds
        }

        // loop through watermark pixels
        for (unsigned int y = 0; y < wmH; y++) {
                for (unsigned int x = 0; x < wmW; x++) {
                        // get watermark pixel color
                        unsigned char wmR = GetPixelR(watermark, x, y);
                        unsigned char wmG = GetPixelG(watermark, x, y);
                        unsigned char wmB = GetPixelB(watermark, x, y);

                        // get pixel from image at same spot (offset by topLeft)
                        unsigned char imagR = GetPixelR(image, topLeftX + x, topLeftY + y);
                        unsigned char imagG = GetPixelG(image, topLeftX + x, topLeftY + y);
                        unsigned char imagB = GetPixelB(image, topLeftX + x, topLeftY + y);

                        // blend the pixels (average them)
                        unsigned char newR = (imagR / 2) + (wmR / 2);
                        unsigned char newG = (imagG / 2) + (wmG / 2);
                        unsigned char newB = (imagB / 2) + (wmB / 2);

                        // update pixel in the image
                        SetPixelR(image, topLeftX + x, topLeftY + y, newR);
                        SetPixelG(image, topLeftX + x, topLeftY + y, newG);
                        SetPixelB(image, topLeftX + x, topLeftY + y, newB);
                }
        }

        return image; // return updated image
}

/* Spotlight */

Image *Spotlight(Image *image, int centerX, int centerY, unsigned int radius){
        if (!image) {
                return NULL; // make sure the image is valid
        }

        unsigned int w = image->W;
        unsigned int h = image->H;

        unsigned int x = 0;
        unsigned int y = 0;

        // loop through every pixel row
        while (y < h) {
                x = 0; // reset column for each row

                // loop through every pixel in the row
                while (x < w) {
                        // calculate distance from current pixel to spotlight center
                        int offsetX = (int)x - centerX;
                        int offsetY = (int)y - centerY;
                        double dist = sqrt(offsetX * offsetX + offsetY * offsetY);

                        // if the pixel is outside the spotlight radius, make it black
                        if (dist > radius) {
                                SetPixelR(image, x, y, 0);
                                SetPixelG(image, x, y, 0);
                                SetPixelB(image, x, y, 0);
                        }

                        x++; // move to next column
                }

                y++; // move to next row
        }
        return image; // return the updated image
}


/* Rotate and zoom an image */
Image *Rotate(Image *image, double angle, double scaleFactor)
{
        if (!image) {
                return NULL; // make sure the image is valid
        }
	
	// convert angle to radians
        double radians = -angle * M_PI / 180.0;
        double cosA = cos(radians);
        double sinA = sin(radians);

	// dimensions
        unsigned int origW = image->W;
        unsigned int origH = image->H;

        // create a new image with the same size
        Image *newImage = CreateImage(origW, origH);

        // calculate center of image
        double centerX = origW / 2.0;
        double centerY = origH / 2.0;

        // loop through pixels in output image
        for (unsigned int y = 0; y < origH; y++) {
                for (unsigned int x = 0; x < origW; x++) {
                        // shift pixel to origin
                        double offsetX = x - centerX;
                        double offsetY = y - centerY;

                        // apply reverse scale and rotation
                        double origX = (cosA * offsetX + sinA * offsetY) / scaleFactor + centerX;
                        double origY = (-sinA * offsetX + cosA * offsetY) / scaleFactor + centerY;

                        // if source pixel is in bounds, copy color otherwise black
                        if (origX >= 0 && origX < origW && origY >= 0 && origY < origH) {
                                unsigned char R = GetPixelR(image, (unsigned int)origX, (unsigned int)origY);
                                unsigned char G = GetPixelG(image, (unsigned int)origX, (unsigned int)origY);
                                unsigned char B = GetPixelB(image, (unsigned int)origX, (unsigned int)origY);

                                SetPixelR(newImage, x, y, R);
                                SetPixelG(newImage, x, y, G);
                                SetPixelB(newImage, x, y, B);
                        } else {
                                SetPixelR(newImage, x, y, 0);
                                SetPixelG(newImage, x, y, 0);
                                SetPixelB(newImage, x, y, 0);
                        }
                }
        }

        return newImage; // return rotated + scaled image
}
