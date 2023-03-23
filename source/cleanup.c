#include "../headers/headers.h"
#include "../headers/cleanup.h"

void cleanUp(LabelNode * labelArray, int labelArraySize, EntryNode * entryArray, EntryNode * labelsToCheckArray) {
    int i;
    
    if(labelArray != NULL) {
        for(i = 0 ; i < labelArraySize ; i++) {
            if(labelArray[i].labelData != NULL)
                free(labelArray[i].labelData);
        }
        free(labelArray);
    }
    
    if(entryArray != NULL)
        free(entryArray);

    if(labelsToCheckArray != NULL)
        free(labelsToCheckArray);
}