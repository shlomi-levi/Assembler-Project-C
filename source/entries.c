#include "../headers/headers.h"
#include "../headers/labels.h"
#include "../headers/entries.h"

/* This function creates the .ent file */
void createEntriesFile(char * fileName, EntryNode * entryArray, int entryArraySize, LabelNode * labelArray, int labelArraySize) {
    char fileToCreate[MAX_LINE_LENGTH];
    char String[MAX_LINE_LENGTH];
    char buffer[BUFFER_SIZE];
    
    int currBufferLen = 0;
    int i;

    FILE * f = NULL;

    addExtension(fileToCreate, fileName, "ent");

    f = fopen(fileToCreate, "w");

    if(!f) {
        printf("Error creating .ent file\n");
        return;
    }

    memset(buffer, 0, BUFFER_SIZE);

    /* Loop through the entries array */
    for(i = 0 ; i < entryArraySize ; i++) {
        int pos;
        
        pos = findLabelPosition(labelArray, labelArraySize, entryArray[i].labelName); /* Find the label's position in the labels array */

        sprintf(String, "%s\t %d\n", entryArray[i].labelName, labelArray[pos].labelAddress); /* Add the label's name + its address to the label array */
        
        addToBuffer(buffer, BUFFER_SIZE, String, &currBufferLen, f);
    }

    if(currBufferLen > 0) /* If the buffer isn't empty, flush it. */
        flushBuffer(buffer, &currBufferLen, f);

    fclose(f);
}