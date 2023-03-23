/* This file is responsible for the second pass of the assembler. It reads each command and write it's encoding to the output file,
In addition to that it encodes the data the source code had (if it had) and creates .externals and .entries file if needed. */

#include "../headers/headers.h"
#include "../headers/encoding.h"
#include "../headers/secondrun.h"
#include "../headers/input.h"
#include "../headers/commands.h"
#include "../headers/cleanup.h"
#include "../headers/externals.h"
#include "../headers/entries.h"

/* This function does the second pass on the source file, with the labels and entry tables */
void secondRun(char * fileName, LabelNode * labelArray, int labelArraySize, EntryNode * entryArray, int entryArraySize) {
    char fileToOpen[MAX_FILENAME_LEN];
    char fileToWriteTo[MAX_FILENAME_LEN];
    char line[MAX_LINE_LENGTH];
    char buffer[BUFFER_SIZE]; /* Buffer array. */
    char temp[MAX_CHARS_PER_COMMAND]; /* This will be used to story commands encodings before adding them to the buffer. */

    LabelNode * externalsArray = NULL; /* A table for the .externals file */
    int externalsArraySize = 0; /* Size of the externals table. we will use this to expand the table when needed */

    int wordCount = FIRST_WORD_INDEX_IN_OUTPUT_FILE; /* Since the first word starts in index 100 */
    int bufferLen = 0;

    FILE * f = NULL;
    FILE * o = NULL;

    commandOutput cmd;
    LineClassification lc;
    
    addExtension(fileToOpen, fileName, "am");
    addExtension(fileToWriteTo, fileName, "ob");

    f = fopen(fileToOpen, "r");
    o = fopen(fileToWriteTo, "w");

    if(!f) {
        printf("Error: failed to open file (File Name: %s)\n", fileToOpen);
        return;
    }
    
    memset(buffer, 0, BUFFER_SIZE); /* Clear the buffer */

    /* Reads a line from the file */
    
    while(fgets(line, MAX_LINE_LENGTH, f) != NULL) {
        removeSpacesFromStart(line);
        removeTrailingSpaces(line);
        
        lc = classifyLine(line, true); /* Classify the line (is it a command, entry line, extern line etc..) */

        /* Since we are in the second pass, we are only interested in lines that are commands. 
        If the current line isn't a command, continue to the next iteration */

        if(lc.type != LINE_COMMAND)
            continue;

        memset(temp, 0, MAX_CHARS_PER_COMMAND); /* Clear temp array */
        
        cmd = parseCommand(lc.commandWithoutLabel); /* Parse the command */

        checkForExternals(cmd, labelArray, labelArraySize, &externalsArray, &externalsArraySize, wordCount); /* Check if the current command has external labels in it */
        
        getCommandEncoding(temp, cmd, labelArray, labelArraySize, &wordCount); /* Get the current command's encoding. */

        addToBuffer(buffer, BUFFER_SIZE, temp, &bufferLen, o); /* Add the current command's encoding to the buffer. */
    }

    if(bufferLen > 0) /* If the buffer isn't empty after we have finished reading the file, flush it. */
        flushBuffer(buffer, &bufferLen, o);

    fclose(f);

    writeDataToFile(o, labelArray, labelArraySize, &wordCount); /* Write the data at the end of the file. */

    fclose(o);

    if(entryArraySize > 0) /* If we have label entries, create .entries file */
        createEntriesFile(fileName, entryArray, entryArraySize, labelArray, labelArraySize);
    
    if(externalsArraySize > 0) /* If we have external labels, create .externals file */
        createExternalsFile(fileName, externalsArray, externalsArraySize);

    if(externalsArray != NULL)
        free(externalsArray); /* Free the external labels array if there are external labels */

    cleanUp(labelArray, labelArraySize, entryArray, NULL); /* Free the label+data table, and the entries table. */
}

/* This function writes all the data the source file had at the end of the object file */
void writeDataToFile(FILE * o, LabelNode * labelArray, int labelArraySize, int * wordCount) {
    char buffer[BUFFER_SIZE]; /* Buffer array. */
    char temp[MAX_LINE_LENGTH]; /* To format strings and add them to the buffer */
    char base2String[WORD_LENGTH];

    int currBufferLength = 0;
    int i;
    int j;

    memset(buffer, 0, BUFFER_SIZE);
    memset(temp, 0, MAX_LINE_LENGTH);
    memset(base2String, 0, WORD_LENGTH);
    
    /* Loop through the labels table */
    for(i = 0 ; i < labelArraySize ; i++) {
        LabelNode l = labelArray[i]; /* get the current label */

        if(!l.isDataLabel) /* If it's not a data label, continue*/
            continue;
        
        for(j = 0 ; j < l.dataCount ; j++) {
            int d = l.labelData[j];
            
            getBase2(base2String, d, NUM_OF_BITS);
            
            sprintf(temp, "%.4d\t%s\n", (*wordCount), base2String); /* Format temp string */
            
            (*wordCount)++; /* Increase the word count */

            addToBuffer(buffer, BUFFER_SIZE, temp, &currBufferLength, o); /* Add temp string to the buffer */
        }
    }

    if(currBufferLength > 0) /* If the buffer isn't empty, flush it */
        flushBuffer(buffer, &currBufferLength, o);
}

