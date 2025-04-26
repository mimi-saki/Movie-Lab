# MovieLab - Digital Image Processing on Video Frames

## Overview

MovieLab is a C-based application designed to apply various digital image processing (DIP) operations to a movie file composed of sequential image frames. The program reads video frame data and performs transformations using a linked list structure to store and manipulate each frame. It supports a variety of operations through command-line arguments and outputs the processed movie in YUV format.

## Features

- Black and white conversion
- Horizontal mirroring
- Edge detection
- Cropping
- Fast-forwarding
- Reversing
- Adding a watermark
- Applying a spotlight effect
- Rotating and zooming frames
- Format conversion between YUV and RGB
- Dynamic memory management for linked movie frame lists

## File Structure

### Source Files
- `MovieLab.c` – Main driver that parses arguments and executes DIP operations
- `Movie.c` / `Movie.h` – Linked list implementation for managing movie frames
- `DIPs.c` / `DIPs.h` – Image processing operations (e.g., crop, mirror, watermark)
- `FileIO.c` / `FileIO.h` – Functions to read/write YUV movie files
- `Image.c` / `Image.h` – RGB image handling functions
- `ImageList.c` / `ImageList.h` – Linked list operations for RGB images
- `ALU.c` / `ALUControl.c` – [May be unrelated if not used; remove if unnecessary]
- `Constants.h` – Definitions and constants used across the project
- `Makefile` – Build script for compiling the project
- `MovieLab.script` – Example script for running MovieLab with arguments

### Output
- `MovieLab.txt` – Project overview and implementation summary

## How to Compile

Use the provided Makefile:

```bash
make
