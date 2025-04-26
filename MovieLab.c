#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "FileIO.h"
#include "DIPs.h"
#include "Movie.h"
#include "Constants.h"
#include "Image.h"

/* Load one movie frame from the input file */
YUVImage* LoadOneFrame(const char* fname, int n,
	unsigned int width, unsigned height);

/* Load the movie frames from the input file */
Movie *LoadMovie(const char *fname, int frameNum,
	unsigned int width, unsigned height);

/* Saves one movie frame to the output file */
void SaveOneFrame(YUVImage *image, const char *fname, FILE *file);

/* Save the movie frames to the output file */
int SaveMovie(const char *fname, Movie *movie);

/* Print the command-line arguments usage of the program */
void PrintUsage();

int main(int argc, char *argv[])
{
        int x = 0;
        char *fin = NULL;
        char *fout = NULL;
        unsigned int frameNum = 0;
        unsigned int width = 0;
        unsigned int height = 0;

        int bw = 0;
        int hmirror = 0;
        int edge = 0;
        int reverse = 0;
        int spotlight = 0;
        int rotate = 0;
        int fastFactor = 0;
        int cropStart = -1;
        int cropEnd = -1;
        char *watermarkFile = NULL;

        /* the while loop to check options in the command line */
        while (x < argc) {
                /* the input file name */
                if (strcmp(argv[x], "-i") == 0) {
                        if (x < argc - 1) {
                                fin = argv[x + 1];
                        } /*fi*/
                        else {
                                printf("Missing argument for the input file name!\n");
                                return 5;
                        } /*esle*/
                        x += 2;
                        continue;
                } /*fi*/ 
                
		/* the output file name */
                if (strcmp(argv[x], "-o") == 0) {
                        if (x < argc - 1) {
                                fout = argv[x + 1];
                        } /*fi*/
                        else {
                                printf("Missing argument for the output file name!\n");
                                return 5;
                        } /*esle*/
                        x += 2;
                        continue;
                } /*fi*/

                // frame
                if (strcmp(argv[x], "-f") == 0) {
                        if (x < argc - 1) {
                                frameNum = atoi(argv[x + 1]);
                        }
                        else {
                                printf("Missing argument for the frame number!\n");
                                return 5;
                        }
                        x += 2;
                        continue;
                }

                // resolution 
                if (strcmp(argv[x], "-s") == 0) {
                        if (x < argc - 1) {
                                sscanf(argv[x + 1], "%ux%u", &width, &height);
                        }
                        else {
                                printf("Missing argument for resolution!\n");
                                return 5;
                        }
                        x += 2;
                        continue;
                }

                // black and white filter
                if (strcmp(argv[x], "-bw") == 0) {
                        bw = 1;
                        x++;
                        continue;
                }

                // horizontal mirror
                if (strcmp(argv[x], "-hmirror") == 0) {
                        hmirror = 1;
                        x++;
                        continue;
                }

                // edge filter
                if (strcmp(argv[x], "-edge") == 0) {
                        edge = 1;
                        x++;
                        continue;
                }

                // crop
                if (strcmp(argv[x], "-crop") == 0) {
                        if (x < argc - 1) {
                                sscanf(argv[x + 1], "%d-%d", &cropStart, &cropEnd);
                        }
                        else {
                                printf("Missing argument for crop range!\n");
                                return 5;
                        }
                        x += 2;
                        continue;
                }

                // fast forward
                if (strcmp(argv[x], "-fast") == 0) {
                        if (x < argc - 1) {
                                fastFactor = atoi(argv[x + 1]);
                        }
                        else {
                                printf("Missing argument for fast forward factor!\n");
                                return 5;
                        }
                        x += 2;
                        continue;
                }

                //reverse
                if (strcmp(argv[x], "-reverse") == 0) {
                        reverse = 1;
                        x++;
                        continue;
                }

                // watermark
                if (strcmp(argv[x], "-watermark") == 0) {
                        if (x < argc - 1) {
                                watermarkFile = argv[x + 1];
                        }
                        else {
                                printf("Missing argument for watermark file!\n");
                                return 5;
                        }
                        x += 2;
                        continue;
                }

                // spotlight
                if (strcmp(argv[x], "-spotlight") == 0) {
                        spotlight = 1;
                        x++;
                        continue;
                }

                // rotate
                if (strcmp(argv[x], "-rotate") == 0) {
                        rotate = 1;
                        x++;
                        continue;
                }

                // help option
                if (strcmp(argv[x], "-h") == 0) {
                        PrintUsage();
                        return 0;
                }

                x++;
        }

	// error codes	
	if (fin == NULL) {
                printf("Missing argument for the input file name!\n");
                PrintUsage();
                return 5;
        }

        if (fout == NULL) {
                printf("Missing argument for the output file name!\n");
                PrintUsage();
                return 5;
        }

        Movie *movie = LoadMovie(fin, frameNum, width, height);
        if (!movie) {
                fprintf(stderr, "Error: Cannot load movie.\n");
                return 2;
        }

        YUV2RGBMovie(movie);

	// menu options 
        if (bw) {
                printf("Operation Black and White is done!\n");
                IENTRY *e = movie->Frames->First;
                while (e) {
                        e->image = BlackNWhite(e->image);
                        e = e->Next;
                }
        }

        if (hmirror) {
                printf("Operation Hmirror is done!\n");
                IENTRY *e = movie->Frames->First;
                while (e) {
                        e->image = HMirror(e->image);
                        e = e->Next;
                }
        }

        if (edge) {
                printf("Operation Edge is done!\n");
                IENTRY *e = movie->Frames->First;
                while (e) {
                        e->image = Edge(e->image);
                        e = e->Next;
                }
        }

        if (cropStart >= 0 && cropEnd >= 0) {
                printf("Operation Crop is done!\n");
                CropImageList(movie->Frames, cropStart, cropEnd);
        }

        if (fastFactor > 1) {
                printf("Operation FastFactor is done!\n");
                FastImageList(movie->Frames, fastFactor);
        }

        if (reverse) {
                printf("Operation Reverse is done!\n");
                ReverseImageList(movie->Frames);
        }

        if (watermarkFile) {
                printf("Operation Watermark is done!\n");
		srand(time(NULL)); // random seed
		int frameCount = 0;
                unsigned int topLeftX = 0;
                unsigned int topLeftY = 0;
		Image *wm = LoadImage(watermarkFile); // load watermark once
                if (!wm) {
                        fprintf(stderr, "Error: Cannot load watermark file!\n");
                        return 3;
                }

                IENTRY *e = movie->Frames->First;
                while (e) { // while we're iterating through the frames in the movie
                        if (frameCount % 15 == 0) { // if its been 15 frames, we change the location of the watermark
                                if (width > wm->W && height > wm->H) {
                                        topLeftX = rand() % (width - wm->W + 1);
                                        topLeftY = rand() % (height - wm->H + 1);
                                } else {
                                        // if watermark is too large, default to top-left
                                        topLeftX = 0;
                                        topLeftY = 0;
                                }
                        }

                        // set  watermark at current position
                        e->image = Watermark(e->image, wm, topLeftX, topLeftY);

                        // next frame
                        e = e->Next;
                        frameCount++;
                }

                DeleteImage(wm); // remove watermark image
        }

        if (spotlight) {
                printf("Operation Spotlight is done!\n");

                // copy edge frames
                Image *firstFrame = CopyRGBImage((Image *)movie->Frames->First->image);
                Image *lastFrame  = CopyRGBImage((Image *)movie->Frames->Last->image);

                // center and max radius
                int centerX = width / 2;
                int centerY = height / 2;
                unsigned int maxRad = (unsigned int)sqrt(centerX * centerX + centerY * centerY);

                // make fade-out frames and append them
                for (int frame = 19; frame >= 0; frame--) {
                        double ratio = (double)frame / 19.0;
                        unsigned int radius = (unsigned int)(maxRad * ratio);
                        Image *copy = CopyRGBImage(lastFrame);
                        Image *outFrame = Spotlight(copy, centerX, centerY, radius);
                        AppendRGBImage(movie->Frames, outFrame);
                }

                // generate fade in frames and collect in array
                Image *setFade[20];
                for (int frame = 0; frame < 20; frame++) {
                        double ratio = (double)frame / 19.0;
                        unsigned int radius = (unsigned int)(maxRad * ratio);
                        Image *copy = CopyRGBImage(firstFrame);
                        setFade[frame] = Spotlight(copy, centerX, centerY, radius);
                }

                // prepend fade in frames in reverse order
                for (int frame = 19; frame >= 0; frame--) {
                        PrependRGBImage(movie->Frames, setFade[frame]);
                }

                // mem clean
                DeleteImage(firstFrame);
                DeleteImage(lastFrame);
        }

        if (rotate) {
                printf("Operation Rotate is done!\n");

                // copy edge frames
                Image *firstFrame = CopyRGBImage((Image *)movie->Frames->First->image);
                Image *lastFrame  = CopyRGBImage((Image *)movie->Frames->Last->image);

                Image *fadeIn[20];
                for (int i = 0; i < 20; i++) {
                        double angle = 20.0 - i; // rotate from 20 degrees to 1 degree
                        double scale = 0.2 + 0.04 * i; // scale 0.2x -> 1.0x
                        Image *copy = CopyRGBImage(firstFrame);
                        fadeIn[i] = Rotate(copy, angle, scale);
			DeleteImage(copy);
                }

                // prepend fade in frames in reverse order
                for (int i = 19; i >= 0; i--) {
                        PrependRGBImage(movie->Frames, fadeIn[i]);
                }
                for (int i = 0; i < 20; i++) {
                        double angle = 1.0 + i; // rotate from 1 degree to 20 degrees 
                        double scale = 1.0 - 0.04 * i; // scale 1.0x to 0.2x
                        Image *copy = CopyRGBImage(lastFrame);
                        Image *rotated = Rotate(copy, angle, scale);
			DeleteImage(copy);
                        AppendRGBImage(movie->Frames, rotated);
                }

                // mem clean
                DeleteImage(firstFrame);
                DeleteImage(lastFrame);
        }

        RGB2YUVMovie(movie);
        SaveMovie(fout, movie);

        DeleteMovie(movie);
        return 0;
}


void PrintUsage()
{
	printf("\nUsage: MovieLab -i <file> -o <file> -f <framenum> [options]\n"
	       "Options:\n"
	       "-s <WidthxHeight>     To set the resolution of the input stream (WidthxHeight)\n"
	       "-bw                   Activate the black and white filter on every movie frame\n"
	       "-hmirror              Activate horizontal mirror on every movie frame\n"
	       "-edge                 Activate the edge filter on every movie frame\n"
	       "-crop <start-end>     Crop the movie frames from <start> to <end>\n"
	       "-fast <factor>        Fast forward the movie by <factor>\n"
	       "-reverse              Reverse the frame order of the input movie\n"
	       "-watermark <file>     Add a watermark from <file> to every movie frame\n"
	       "-spotlight            Spotlight the fade in and fade out\n"
	       "-rotate               Rotate and zoom in/out the input movie\n"
	       "-h                    Display this usage information\n"
	      );
}

/* Load one movie frame from the input file */
YUVImage* LoadOneFrame(const char* fname, int n,
	unsigned int width, unsigned height)
{
	FILE *file;
	unsigned int x, y;
	unsigned char c;
	YUVImage* YUVimage;

	/* Check errors */
	assert(fname);
	assert(n >= 0);

	YUVimage = CreateYUVImage(width, height);
	if (YUVimage == NULL) {
		return NULL;
	}

	/* Open the input file */
	file = fopen(fname, "r");
	if (file == NULL) {
		DeleteYUVImage(YUVimage);
		return NULL;
	}

	/* Find the desired frame */
	fseek(file, 1.5 * n * width * height, SEEK_SET);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			c = fgetc(file);
			SetPixelY(YUVimage, x, y, c);
		} /*rof*/
	}

	for (y = 0; y < height; y += 2) {
		for (x = 0; x < width; x += 2) {
			c = fgetc(file);
			SetPixelU(YUVimage, x, y, c);
			SetPixelU(YUVimage, x + 1, y, c);
			SetPixelU(YUVimage, x, y + 1, c);
			SetPixelU(YUVimage, x + 1, y + 1, c);
		}
	}

	for (y = 0; y < height; y += 2) {
		for (x = 0; x < width; x += 2) {
			c = fgetc(file);
			SetPixelV(YUVimage, x, y, c);
			SetPixelV(YUVimage, x + 1, y, c);
			SetPixelV(YUVimage, x, y + 1, c);
			SetPixelV(YUVimage, x + 1, y + 1, c);
		}
	}

	/* Check errors */
	assert(ferror(file) == 0);

	/* Close the input file and return */
	fclose(file);
	file = NULL;
	return YUVimage;
}

/* Load the movie frames from the input file */
Movie *LoadMovie(const char *fname, int frameNum,
                unsigned int width, unsigned height)
{
        if (!fname || frameNum <= 0 || width == 0 || height == 0) {
                return NULL;
        }

        Movie *movie = CreateMovie();
        if (!movie) {
                fprintf(stderr, "Error: Could not allocate memory for Movie.\n");
                return NULL;
        }

        for (int i = 0; i < frameNum; i++) {
                YUVImage *frame = LoadOneFrame(fname, i, width, height);
                if (!frame) {
                        fprintf(stderr, "Error: Failed to load frame %d from %s.\n", i, fname);
                        DeleteMovie(movie);
                        return NULL;
                }
                AppendYUVImage(movie->Frames, frame);
        }

        return movie;
}

/* Save the movie frames to the output file */
int SaveMovie(const char *fname, Movie *movie)
{
	int count;
	FILE *file;
	IENTRY *curr;

	/* Open the output file */
	file = fopen(fname, "w");
	if (file == NULL) {
		return 1;
	}

	count = 0;
	curr = movie->Frames->First;
	while (curr != NULL) {

		SaveOneFrame(curr->image, fname, file);
		curr = curr->Next;
		count++;
	}

	fclose(file);
	file = NULL;

	printf("The movie file %s has been written successfully!\n", fname);
	printf("%d frames are written to the file %s in total.\n", count, fname);
	return 0;
}

/* Saves one movie frame to the output file */
void SaveOneFrame(YUVImage *image, const char *fname, FILE *file)
{
	int x, y;
	for (y = 0; y < image->H; y++) {
		for (x = 0; x < image->W; x++) {
			fputc(GetPixelY(image, x, y), file);
		}
	}

	for (y = 0; y < image->H; y += 2) {
		for (x = 0; x < image->W; x += 2) {
			fputc(GetPixelU(image, x, y), file);
		}
	}

	for (y = 0; y < image->H; y += 2) {
		for (x = 0; x < image->W; x += 2) {
			fputc(GetPixelV(image, x, y), file);
		}
	}
}
