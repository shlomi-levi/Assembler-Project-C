#include "../headers/headers.h"
#include "../headers/labels.h"
#include "../headers/externals.h"
#include "../headers/commands.h"

void checkForExternals(LabelNode * labelArray, int labelArraySize, EntryNode ** externalsArray, int * externalsArraySize, char * param, int address) {
    int labelPos;
        
    labelPos = findLabelPosition(labelArray, labelArraySize, param);
    
    if(!labelArray[labelPos].isExternal)
        return;

    addEntry(externalsArray, (*externalsArraySize), param, address);

    (* externalsArraySize) ++;
}

void createExternalsFile(char * fileName, EntryNode * externalsArray, int externalsArraySize) {
    char fileToCreate[MAX_LINE_LENGTH];
    char String[MAX_LINE_LENGTH];
    char buffer[BUFFER_SIZE];
    
    int currBufferLen = 0;
    int i;

    FILE * f = NULL;

    addExtension(fileToCreate, fileName, "externals");

    f = fopen(fileToCreate, "w");

    if(!f) {
        printf("Error creating .externals file\n");
        return;
    }

    memset(buffer, 0, BUFFER_SIZE);

    for(i = 0 ; i < externalsArraySize ; i++) {
        sprintf(String, "%s\t %d\n", externalsArray[i].labelName, externalsArray[i].labelLine);
        
        addToBuffer(buffer, BUFFER_SIZE, String, &currBufferLen, f);
    }

    if(currBufferLen > 0)
        flushBuffer(buffer, &currBufferLen, f);

    fclose(f);
}