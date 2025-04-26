#include <stdlib.h>
#include <assert.h>
#include "ImageList.h"

/* Create a new image list */
ILIST *CreateImageList(void)
{
        ILIST *newList = (ILIST *)malloc(sizeof(ILIST)); // allocate memory for new list
        if (!newList) {
                return NULL; // malloc failed
        }

        newList->Length = 0; // set initial length
        newList->First = NULL; // no first entry yet
        newList->Last = NULL; // no last entry yet
        return newList; // return the new list
}

/* Delete an image list (and all entries) */
void DeleteImageList(ILIST *list)
{
        IENTRY *current = list->First; // start at first entry

        while (current) {
                IENTRY *next = current->Next; // store next entry
                DeleteYUVImage((YUVImage *)current->image); // free image data
                free(current); // free entry node
                current = next; // move to next
        }

        free(list); // free list structure
}

/* Insert a RGB image to the image list at the end */
void AppendRGBImage(ILIST *list, Image *RGBimage)
{
        if (!list || !RGBimage) { // check if list or image is NULL
                return;
        }

        IENTRY *newSpace = (IENTRY *)malloc(sizeof(IENTRY)); // allocate memory for new entry
        if (!newSpace) {
                return; // malloc failed
        }

        newSpace->image = (void *)RGBimage; // store RGB image
        newSpace->Next = NULL; // new entry goes at the end
        newSpace->Prev = list->Last;
        newSpace->List = list;

        if (list->Last) {
                list->Last->Next = newSpace; // link from old last to new
        }

        else {
                list->First = newSpace; // if empty, set as first
        }

        list->Last = newSpace; // update last to new entry
        list->Length++; // increment list length
}

/* Insert a YUV image to the image list at the end */
void AppendYUVImage(ILIST *list, YUVImage *YUVimage)
{
        if (!list || !YUVimage) { // check if list or image is NULL
                return;
        }

        IENTRY *newSpace = (IENTRY *)malloc(sizeof(IENTRY)); // allocate memory for new entry

        if (!newSpace) {
                return; // malloc failed
        }

        newSpace->image = (void *)YUVimage; // store YUV image
        newSpace->Next = NULL; // new entry goes at the end
        newSpace->Prev = list->Last;
        newSpace->List = list;

        if (list->Last) {
                list->Last->Next = newSpace; // link from old last to new
        }

        else {
                list->First = newSpace; // if empty, set as first
        }

        list->Last = newSpace; // update last to new entry
        list->Length++; // increment list length
}


/* Crop an image list */
void CropImageList(ILIST *list, unsigned int start, unsigned int end)
{
        if (!list || start >= end || list->Length == 0) { // check if list is NULL, start is after end, or list is empty
                return;
        }
	
	unsigned int frameIndex = 0; // frame index
        IENTRY *current = list->First; // start at first frame

        while (current) {
                IENTRY *next = current->Next; // store next frame

                // remove frames not within the start, end range
                if (frameIndex < start || frameIndex > end) {
                        if (current->Prev) {
                                current->Prev->Next = current->Next;
                        }

                        if (current->Next) {
                                current->Next->Prev = current->Prev;
                        }

                        if (current == list->First) {
                                list->First = current->Next;
                        }

                        if (current == list->Last) {
                                list->Last = current->Prev;
                        }

                        DeleteImage(current->image);
                        free(current);
                        list->Length--; // update list length
                }

                frameIndex++; // increment frame index
                current = next; // move to next frame
        }
}

/* Fast image list */ 
void FastImageList(ILIST *list, unsigned int factor)
{
        if (!list || factor < 1 || list->Length == 0) { // check if list is NULL, factor is invalid, or list is empty
                return;
        }

	unsigned int frameIndex = 0; // frame index
        IENTRY *current = list->First; // start at first frame
        
	while (current) {
                IENTRY *next = current->Next; // holds next frame

                // keep only frames at positions that are multiples of factor
                if (frameIndex % factor != 0) {
                        if (current->Prev) {
                                current->Prev->Next = current->Next;
                        }

                        if (current->Next) {
                                current->Next->Prev = current->Prev;
                        }

                        if (current == list->First) {
                                list->First = current->Next;
                        }

                        if (current == list->Last) {
                                list->Last = current->Prev;
                        }

                        DeleteImage(current->image);
                        free(current);
                        list->Length--; // update list length
                }

                frameIndex++; // increment frame index
                current = next; // move to next frame
        }
}

/* Reverse an image list */
void ReverseImageList(ILIST *list)
{
        if (!list || list->Length <= 1) { // check if list is null/0-1 frame 
                return; // no reverse
        }

        IENTRY *current = list->First; // start from the first frame in the list
        IENTRY *temp = NULL; // temp pointer 

        while (current) { // loop through list to flip next/previous nodes 
                temp = current->Next; // store next node
                current->Next = current->Prev; // swap next and previous node 
                current->Prev = temp;
                current = temp; // move to next 
        }

        temp = list->First; // swap first and last pointers
        list->First = list->Last;
        list->Last = temp;
}

/* Insert a RGB image to the image list in the front */
void PrependRGBImage(ILIST *list, Image *RGBimage)
{
        if (!list || !RGBimage) { // check if image exists
                return;
        }

        IENTRY *newSpace = (IENTRY *)malloc(sizeof(IENTRY)); // make new entry space
        if (!newSpace) {
                return; // if it fails, return nothing
        }

        // create new entry
        newSpace->image = (void *)RGBimage;
        newSpace->Next = list->First;
        newSpace->Prev = NULL;
        newSpace->List = list;

        if (list->First) { // chceck if list is empty, if its empty link old first to new
                list->First->Prev = newSpace;
        }

        else {
                list->Last = newSpace;
        }

        // new entry as first
        list->First = newSpace;

        // incrament by 1
        list->Length++;
}
