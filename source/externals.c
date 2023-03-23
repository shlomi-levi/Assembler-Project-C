#include "../headers/headers.h"
#include "../headers/labels.h"
#include "../headers/externals.h"
#include "../headers/commands.h"

void checkForExternals(commandOutput cmd, LabelNode * labelArray, int labelArraySize, LabelNode ** externalsArray, int * externalsArraySize, int wordCount) {
    enum addressingModes am;

    char * stuffToCheck [3];

    int i;
    int labelPos;

    stuffToCheck[0] = cmd.srcParameter;
    stuffToCheck[1] = cmd.destParameter;
    stuffToCheck[2] = cmd.jumpLabel;

    for(i = 0 ; i < 3 ; i++) {
        char * curr = stuffToCheck[i];
        
        if(curr[0] == '\0')
            continue;
        
        am = getAddressingMode(curr);

        if(am != addressingMode_DIRECT)
            continue;
        
        labelPos = findLabelPosition(labelArray, labelArraySize, curr);
        if(!labelArray[labelPos].isExternal)
            continue;

        addLabel(externalsArray, (*externalsArraySize), curr, false, wordCount);

        (* externalsArraySize) ++;
    }
}

void createExternalsFile(char * fileName, LabelNode * externalsArray, int externalsArraySize) {
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
        sprintf(String, "%s\t %d\n", externalsArray[i].labelName, externalsArray[i].labelAddress);
        
        addToBuffer(buffer, BUFFER_SIZE, String, &currBufferLen, f);
    }

    if(currBufferLen > 0)
        flushBuffer(buffer, &currBufferLen, f);

    fclose(f);
}