#include "../headers/headers.h"
#include "../headers/cleanup.h"

/* This function is responsible for memory cleanup */
void cleanUp(LabelNode * labelArray, int labelArraySize, EntryNode * entryArray, EntryNode * labelsToCheckArray) {
    int i;
    
    /* Loop through the labels array */
    if(labelArray != NULL) {
        for(i = 0 ; i < labelArraySize ; i++) { /* For each label, if that label has data, free the data as well*/
            if(labelArray[i].labelData != NULL)
                free(labelArray[i].labelData);
        }
        free(labelArray); /* Free the label array*/
    }
    
    if(entryArray != NULL)
        free(entryArray);

    if(labelsToCheckArray != NULL)
        free(labelsToCheckArray);
}