/* This file is responsible for the preprocessing phase. It processes the macros and parses them,
eventually creating the .am file */

#include "../headers/headers.h"
#include "../headers/commands.h"
#include "../headers/macros.h"
#include "../headers/firstrun.h"
#include "../headers/preprocessor.h"

/* This function recieves files names and checks wether they exist and can be opened,
If a file doesn't exist or cannot be opened for some reason, an appropriate message would be displayed */
void checkIfFilesExist(int argc, char * argv[]) {
    FILE * f;
    int i;
    char fileName[MAX_FILENAME_LEN];

    /* The first argument is the executables name, we don't need it. */
    for(i = 1 ; i < argc ; i++ ) {
        addExtension(fileName, argv[i], "as"); /* Add the appropriate extension to the file's name. */

        f = fopen(fileName, "r");

        if(!f)
            printf("File not found / Unable to open file (File name: %s)\n", fileName);

        else
            fclose(f);
    }

    preProcessFiles(argc, argv);
}

void preProcessFiles(int argc, char * argv[]) {
    int i;
    /* The first argument is the executables name, we don't need it. */
    for(i = 1 ; i < argc ; i++) 
        preProcessFile(argv[i]);
}

void preProcessFile(char * fileName) {
    FILE * input = NULL;
    FILE * output = NULL;

    MacroNode * macroTable; /* Macro Table */
    MacroNode currentMacro; /* Current Macro we're working on */

    bool macroDefinitionInProcess = false;
    bool validFile = true;
    bool hasMacros = false;

    char fileToOpen[MAX_FILENAME_LEN];
    char fileToWriteTo[MAX_FILENAME_LEN];

    char line[MAX_LINE_LENGTH];
    char temp[MAX_LINE_LENGTH];
    char buffer[BUFFER_SIZE];

    char * line2; /* A pointer to the current line, I'm using it to parse the line word by word */

    int lineNumber = 0; /* line number variable so that in case of errors I would be able to print the number of line in which the error occured. */

    int macroCount = 0;

    int macroLocation = NOT_IN_MACRO_TABLE;

    int currentBufferLength = 0;

    int i; /* Iterator */

    addExtension(fileToOpen, fileName, "as");
    addExtension(fileToWriteTo, fileName, "am");
    
    input = fopen(fileToOpen, "r");

    if(!input)
        return;

    output = fopen(fileToWriteTo, "w");

    if(!output) {
        printf("Failed to open/create file (%s)", fileToWriteTo);
    
        fclose(input);

        return;
    }

    memset(buffer, 0, BUFFER_SIZE);

    /* Reads a line from the file */
    while(fgets(line, MAX_LINE_LENGTH, input) != NULL) {
        lineNumber++;

        removeRedundantSpaces(line);

        strcat(line, "\n"); /* Removing the spaces also removed the \n character so I'll add it. */
        
        if(isEmptyLine(line) || isComment(line)) /* Ignore comments and empty lines*/
            continue;

        line2 = line;

        if(macroDefinitionInProcess) { /* If there's currently a macro definition */
            if(!macroEnding(line)) { /* If the current line isnt 'endmcr', then add the current line to the current macro's definition */
                addToMacroDefinition(&currentMacro, line);
                continue;
            }

            /* If this is the end of the macro, add the macro to the macro's table and skip the rest of the iteration (continue) */

            macroCount++;
            
            macroTable = (hasMacros) ? my_realloc(macroTable, sizeof(MacroNode) * macroCount) : my_malloc(sizeof(MacroNode)); /* Expand macro table size */

            hasMacros = true;

            macroTable[macroCount - 1] = currentMacro;

            macroDefinitionInProcess = false;
            
            continue;
        }

        if(!isMacroDefinitionStatement(line)) { /* If this isn't a macro definition statement, go through each word in the line and where there are macros parse them */
            while(getWord(temp, MAX_LINE_LENGTH, &line2)) {
                if(isEmptyLine(temp))
                    continue;
                
                macroLocation = isExistingMacro(macroTable, temp, macroCount);

                strcat(temp, " ");

                if(macroLocation == NOT_IN_MACRO_TABLE)
                    addToBuffer(buffer, BUFFER_SIZE, temp, &currentBufferLength, output);

                else
                    addToBuffer(buffer, BUFFER_SIZE, macroTable[macroLocation].definition, &currentBufferLength, output);
            }

            addToBuffer(buffer, BUFFER_SIZE, "\n", &currentBufferLength, output);
            
            continue;
        }
        
        /* Now this is a macro definition */

        if(!isLegalMacroName(line2)) {
            printf("Illegal macro name in file (%s) on line (%d)", fileToOpen, lineNumber);
            validFile = false;
        }

        currentMacro = createEmptyMacro();
    
        getMacroName(currentMacro.name, MAX_LINE_LENGTH, line); /* Get macro name returns the name without trailing spaces */
        
        currentMacro.definition = my_calloc(MAX_LINE_LENGTH, sizeof(char));
        currentMacro.size = MAX_LINE_LENGTH;

        macroDefinitionInProcess = true;

        continue;
    }

    if(currentBufferLength > 0) /* Flush buffer if the buffer currently holds data */
        flushBuffer(buffer, &currentBufferLength, output);
    
    fclose(input);
    fclose(output);

    /* Free all the memory allocations we have used */

    if(macroTable != NULL) {
        for(i = 0 ; i < macroCount ; i++) {
            if(macroTable[i].definition != NULL)
                free(macroTable[i].definition);
        }
        free(macroTable);
    }
        
    if(!validFile) { /* If the .as file had errors, remove the output file (the '.am' file). */
        remove(fileToWriteTo);
        return;
    }

    firstRun(fileName);
}