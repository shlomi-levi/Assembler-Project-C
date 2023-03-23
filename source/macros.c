#include "../headers/headers.h"
#include "../headers/commands.h"
#include "../headers/macros.h"

bool isMacroDefinitionStatement(char * line) {
    if(!startsWith(line, "mcr "))
        return false;
    
    return true;
}

void getMacroName(char * word, int wordLen, char * line) {
    getWord(word, wordLen, &line); /* This is mcr */
    getWord(word, wordLen, &line); /* This is the desired macro's name */
}

MacroNode createEmptyMacro() {
    MacroNode e;
    
    e.size = 0;
    memset(e.name, 0, MAX_LINE_LENGTH);
    e.definition = NULL;

    return e;
}

int isExistingMacro(MacroNode * mt, char * macro_name, int macroCount) {
    int i;

    char macroNameWithoutTrailingSpaces[MAX_LINE_LENGTH];

    strcpy(macroNameWithoutTrailingSpaces, macro_name);

    removeTrailingSpaces(macroNameWithoutTrailingSpaces);

    for(i = 0 ; i < macroCount ; i++) {
        if(!strcmp(mt[i].name, macroNameWithoutTrailingSpaces))
            return i;
    }

    return NOT_IN_MACRO_TABLE;
}

bool macroEnding(char * str) {
    char word[MAX_LINE_LENGTH];

    getWord(word, MAX_LINE_LENGTH, &str);

    if(!strcmp(word, "endmcr") && isEmptyLine(str))
        return true;

    return false;
}

bool isLegalMacroName(char * line) {
    char name[MAX_LINE_LENGTH];
    
    getWord(name, MAX_LINE_LENGTH, &line); /* Remove the "mcr" from the line */ 
    getWord(name, MAX_LINE_LENGTH, &line); /* get the desired macro's name */

    if(!isEmptyLine(line))
        return false;
    
    if(isCommand(name))
        return false;

    return true;
}

void addToMacroDefinition(MacroNode * macro, char * add) {
    int currentLen = strlen(macro->definition);
    int addLen = strlen(add);

    if( (macro->size) - currentLen < (addLen + 1) ) {
        macro->definition = my_realloc(macro->definition, macro->size + addLen + 1);
        macro->size += addLen + 1;
    }
    
    strcat(macro->definition, add);
}