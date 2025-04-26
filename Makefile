# Variables
CC = gcc
CFLAGS = -Wall -std=c11 -g

# Default target
all: MovieLab

######################### Generate object files #######################

# Target for MovieLab.o
MovieLab.o: MovieLab.c Movie.h ImageList.h FileIO.h DIPs.h
	$(CC) $(CFLAGS) -c MovieLab.c -o MovieLab.o

# Target for FileIO.o
FileIO.o: FileIO.c FileIO.h
	$(CC) $(CFLAGS) -c FileIO.c -o FileIO.o

# Target for DIPs.o
DIPs.o: DIPs.c DIPs.h Image.h
	$(CC) $(CFLAGS) -c DIPs.c -o DIPs.o

# Target for Image.o
Image.o: Image.c Image.h
	$(CC) $(CFLAGS) -c Image.c -o Image.o

# Target for ImageList.o
ImageList.o: ImageList.c ImageList.h Image.h
	$(CC) $(CFLAGS) -c ImageList.c -o ImageList.o

# Target for Movie.o
Movie.o: Movie.c Movie.h ImageList.h Image.h
	$(CC) $(CFLAGS) -c Movie.c -o Movie.o

######################### Generate the executable #####################
# Target for MovieLab
MovieLab: MovieLab.o FileIO.o DIPs.o Image.o ImageList.o Movie.o
	$(CC) $(CFLAGS) -o MovieLab MovieLab.o FileIO.o DIPs.o Image.o ImageList.o Movie.o -lm

###############################  others  ##############################
# Target for clean-up
clean:
	rm -f MovieLab.o FileIO.o DIPs.o Image.o ImageList.o Movie.o MovieLab *.yuv
