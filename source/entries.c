#include "../headers/headers.h"
#include "../headers/labels.h"
#include "../headers/entries.h"

void createEntriesFile(char * fileName, EntryNode * entryArray, int entryArraySize, LabelNode * labelArray, int labelArraySize) {
    char fileToCreate[MAX_LINE_LENGTH];
    char String[MAX_LINE_LENGTH];
    char buffer[BUFFER_SIZE];
    
    int currBufferLen = 0;
    int i;

    FILE * f = NULL;

    addExtension(fileToCreate, fileName, "entries");

    f = fopen(fileToCreate, "w");

    if(!f) {
        printf("Error creating .entries file\n");
        return;
    }

    memset(buffer, 0, BUFFER_SIZE);

    for(i = 0 ; i < entryArraySize ; i++) {
        int pos;
        
        pos = findLabelPosition(labelArray, labelArraySize, entryArray[i].labelName);

        if(pos == NOT_FOUND) {
            printf("Error. label `%s` declared as entry and not found in symbol table\n ", entryArray[i].labelName);
            continue;
        }

        sprintf(String, "%s\t %d\n", entryArray[i].labelName, labelArray[pos].labelAddress);
        
        addToBuffer(buffer, BUFFER_SIZE, String, &currBufferLen, f);
    }

    if(currBufferLen > 0)
        flushBuffer(buffer, &currBufferLen, f);

    fclose(f);
}