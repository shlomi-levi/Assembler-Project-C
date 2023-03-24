/* This file is responsible for the first run */

#include "../headers/headers.h"
#include "../headers/firstrun.h"
#include "../headers/secondrun.h"
#include "../headers/labels.h"
#include "../headers/input.h"
#include "../headers/commands.h"
#include "../headers/cleanup.h"

/* First run function */
void firstRun(char * fileName) {

    char fileToOpen[MAX_FILENAME_LEN];
    char line[MAX_LINE_LENGTH];
    char * problems = NULL;

    FILE * f = NULL;

    /* Label, entry, and labels to check arrays */
    
    LabelNode * labelArray = NULL;
    EntryNode * entryArray = NULL;
    EntryNode * labelsToCheckArray = NULL;

    int labelArraySize = 0;
    int entryArraySize = 0;
    int labelsToCheckSize = 0;
    
    /* */
    
    bool validFile = true;
    int lineNumber = 0;
    int wordCount = FIRST_WORD_INDEX_IN_OUTPUT_FILE;

    int endingOfCommandsEncoding;
    int dataWordsCount;
    
    int l;
    int e;

    LineClassification lc;
    commandOutput cmd;

    addExtension(fileToOpen, fileName, "am");

    f = fopen(fileToOpen, "r");
    
    if(!f) {
        printf("Error: failed to open file (File Name: %s)\n", fileToOpen);
        return;
    }

    /* Reads a line from the file */

    memset(line, 0, MAX_LINE_LENGTH);
    
    while(fgets(line, MAX_LINE_LENGTH, f) != NULL) {
        lineNumber++; /* Advance the line number */
        
        removeRedundantSpaces(line); /* Get rid of spaces from start and end. */

        lc = classifyLine(line, false); /* Classify the line */
        
        if(lc.type == LINE_EMPTY || lc.type == LINE_COMMENT)
            continue;

        if(lc.labelForExtern)
            printf("Warning: on file (%s) on line (%d) - trying to declare a label for an extern statement\n", fileToOpen, lineNumber);

        if(lc.labelForEntry)
            printf("Warning: on file (%s) on line (%d) - trying to declare a label for an entry statement\n", fileToOpen, lineNumber);

        
        switch(lc.type) {
            case LINE_INVALID:
                validFile = false;
                printf("Error in file (%s) on line (%d) - %s\n", fileToOpen, lineNumber, lc.errorMessage);
                continue;
        
            case LINE_EXTERN:
                l = findLabelPosition(labelArray, labelArraySize, lc.label);
            
                /* If the external label already exists in the labels array */
                if(l != NOT_FOUND) {

                    if(labelArray[l].isExternal) {
                        printf("Warning: in file (%s) on line (%d) - label `%s` is already defined as an external label. This line is redundant.\n", fileToOpen, lineNumber, lc.label);
                        continue;
                    }
                    
                    printf("Error in file (%s) on line (%d) - label `%s` is already defined as not external.\n", fileToOpen, lineNumber, lc.label);
                    validFile = false;
                    continue;
                }

                /* If the external label already exists in the entries array */

                e = findEntryPosition(entryArray, entryArraySize, lc.label);
                
                if(e != NOT_FOUND) {
                    printf("Error in file (%s) on line (%d) - label `%s` is already defined as entry. cannot be defined as external as well.\n",
                    fileToOpen, lineNumber, lc.label);

                    validFile = false;
                    continue;
                }

                /* Otherwise, add the label as external */
                addLabel(&labelArray, labelArraySize, lc.label, true, 0);

                labelArraySize++;

                continue;

            case LINE_ENTRY:
                /* Similar to the handling of LINE_EXTERN */
                l = findLabelPosition(labelArray, labelArraySize, lc.label);
                
                if(l != NOT_FOUND) {
                    if(labelArray[l].isExternal) {
                        printf("Error in file(%s) on line (%d) - label `%s` is already declared as external.\n", fileToOpen, lineNumber, lc.label);
                        validFile = false;
                        continue;
                    }
                }

                e = findEntryPosition(entryArray, entryArraySize, lc.label);

                if(e != NOT_FOUND) {
                    printf("Warning: in file (%s) on line (%d) - label `%s` is already flagged as an entry.\n", fileToOpen, lineNumber, lc.label);
                    continue;
                }

                addEntry(&entryArray, entryArraySize, lc.label, lineNumber);
                entryArraySize++;
                continue;
            
            case LINE_VARIABLE:
                if(!strlen(lc.label) || findLabelPosition(labelArray, labelArraySize, lc.label) == NOT_FOUND) { /* If the variable has no name, or it has a name and it isn't in the label array */
                    addDataLabel(&labelArray, labelArraySize, lc.label, lc.data, lc.dataLength);
                    labelArraySize++;
                    continue;
                }

                /* If the variable is already in the labels array */
                validFile = false;

                if(labelArray[l].isExternal)
                    printf("Error in file (%s) on line (%d) - label `%s` is already defined as an external label\n", fileToOpen, lineNumber, lc.label);

                printf("Error in file (%s) on line (%d) - label `%s` is already defined\n", fileToOpen, lineNumber, lc.label);
                validFile = false;

                continue;
            
            default: break;                    
        }
        /* The only option left is that the line is a command */

        cmd = parseCommand(lc.commandWithoutLabel); /* Parse the command */
        
        /* If the command is invalid */
        if(!cmd.validCommand) {
            validFile = false;
            printf("Error in file (%s) on line (%d) - %s\n", fileToOpen, lineNumber, cmd.errorMessage);
            continue;
        }
        
        /* If the command is a jump command and the jump parameter is a label, we need to add its name to the labelsToCheckArray */
        if(isJumpCommand(cmd.commandType)) {
            if(getAddressingMode(cmd.jumpLabel) == addressingMode_DIRECT) {
                addEntry(&labelsToCheckArray, labelsToCheckSize, cmd.jumpLabel, lineNumber);
                labelsToCheckSize++;
            }
        }
        
        /* If the command has a label */
        if(lc.label[0] != '\0') {
            int labelPos = findLabelPosition(labelArray, labelArraySize, lc.label);

            /* If this label isn't in the data/commands/external labels array */
            if(labelPos == NOT_FOUND) {
                addLabel(&labelArray, labelArraySize, lc.label, false, wordCount);
                labelArraySize++;
            }

            /* If this label already exists in the data/commands/external labels array */
            else {
                validFile = false;

                if(labelArray[labelPos].isExternal)
                    printf("Error in file (%s) on line (%d) - label `%s` is already defined as an external label.", fileToOpen, lineNumber, lc.label);
                
                else if(labelArray[labelPos].isDataLabel)
                    printf("Error in file (%s) on line (%d) - label `%s` is already defined as a data label.", fileToOpen, lineNumber, lc.label);

                else
                    printf("Error in file (%s) on line (%d) - label `%s` is already defined as a command label", fileToOpen, lineNumber, lc.label);
                
                continue;
            }
        }

        /* If the src/dest parameters are labels, we need to add them to the labelsToCheckArray */
        if(cmd.srcAddressing == addressingMode_DIRECT) {
            addEntry(&labelsToCheckArray, labelsToCheckSize, cmd.srcParameter, lineNumber);
            labelsToCheckSize++;
        }

        if(cmd.destAddressing == addressingMode_DIRECT) {
            addEntry(&labelsToCheckArray, labelsToCheckSize, cmd.destParameter, lineNumber);
            labelsToCheckSize++;
        }

        wordCount += cmd.numOfWords; /* Increase wordcount */
    }

    fclose(f);

    /* If there are problematic labels, get a string that contains them (seperated with newlines) */
    problems = getProblematicLabels(labelArray, labelArraySize, entryArray, entryArraySize, labelsToCheckArray, labelsToCheckSize);

    if(problems != NULL) {
        printf("Error in file (%s) - The following labels weren't declared in the source code but were used as paraemters/entry labels: \n",
        fileToOpen);
        
        printf("%s", problems);

        free(problems);

        validFile = false;
    }

    printf("\n\n");
    
    /* If the assembly code already had errors in it, or the last line is an error, we don't need
    to create output files */

    if(!validFile) {
        cleanUp(labelArray, labelArraySize, entryArray, labelsToCheckArray);
        return;
    }

    if(labelsToCheckArray != NULL)
        free(labelsToCheckArray);

    endingOfCommandsEncoding = wordCount; /* Get the id of the first word that doesn't belong to a command */
    
    dataWordsCount = addAddressesToDataLabels(labelArray, labelArraySize, &wordCount); /* Add addresses to variables and get the number of words that these labels occupy at the end of the output file */

    secondRun(fileName, labelArray, labelArraySize, entryArray, entryArraySize, endingOfCommandsEncoding, dataWordsCount);
}