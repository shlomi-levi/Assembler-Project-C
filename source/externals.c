#include "../headers/headers.h"
#include "../headers/labels.h"
#include "../headers/externals.h"
#include "../headers/commands.h"

/* This function recieves the labels array and its length,
an additional array named externals that has an entry for each time that a memory word refers to an external label
(each entry has the external label's name and the address of the word that refers to it),
a label name param and an address that this label appears in. It checks if the label with the name param
is external, and if so, it adds an entry in the externalsArray with that labels name and the address that is in the address param */

void checkForExternals(LabelNode * labelArray, int labelArraySize, EntryNode ** externalsArray, int * externalsArraySize, char * param, int address) {
    int labelPos;
        
    labelPos = findLabelPosition(labelArray, labelArraySize, param); /* Find a label with label name param in the labels array */
    
    if(!labelArray[labelPos].isExternal) /* If that label is not external, return */
        return;

    addEntry(externalsArray, (*externalsArraySize), param, address); /* Add the entry */

    (* externalsArraySize) ++; /* Increase externals array size variable */
}

/* This function creates the .externals file, using the externals array */

void createExternalsFile(char * fileName, EntryNode * externalsArray, int externalsArraySize) {
    char fileToCreate[MAX_LINE_LENGTH];
    char String[MAX_LINE_LENGTH];
    char buffer[BUFFER_SIZE];
    
    int currBufferLen = 0;
    int i;

    FILE * f = NULL;

    addExtension(fileToCreate, fileName, "ext");

    f = fopen(fileToCreate, "w");

    if(!f) {
        printf("Error creating .ext file\n");
        return;
    }

    memset(buffer, 0, BUFFER_SIZE);
    
    /* Loop through the externals array and add each label with the address to the buffer */
    for(i = 0 ; i < externalsArraySize ; i++) {
        sprintf(String, "%s\t %d\n", externalsArray[i].labelName, externalsArray[i].labelLine);
        
        addToBuffer(buffer, BUFFER_SIZE, String, &currBufferLen, f);
    }

    if(currBufferLen > 0) /* If the buffer holds data, flush it. */
        flushBuffer(buffer, &currBufferLen, f);

    fclose(f);
}