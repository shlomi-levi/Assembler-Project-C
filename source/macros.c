/* This file contains helpful methods to deal with macros */

#include "../headers/headers.h"
#include "../headers/commands.h"
#include "../headers/macros.h"

/* This function recieves a string and returns true if this string is a macro definition statement (basically, wether
the string starts with 'mcr ' or not). */

bool isMacroDefinitionStatement(char * line) {
    if(!startsWith(line, "mcr "))
        return false;
    
    return true;
}

/* This function recieves a line (already assumed to be a macro definition statement) and puts the macro's name in the address in which the pointer 'word' points to */

void getMacroName(char * word, int wordLen, char * line) {
    getWord(word, wordLen, &line); /* This is mcr */
    getWord(word, wordLen, &line); /* This is the desired macro's name */
}

/* This function creates an empty macro */

MacroNode createEmptyMacro() {
    MacroNode e;
    
    e.size = 0;
    memset(e.name, 0, MAX_LINE_LENGTH);
    e.definition = NULL;

    return e;
}

/* This function recieves the macro table and the number of macros in that table, and returns the index in which the macro with the name
that is in "macro_name" sits in, or NOT_IN_MACRO_TABLE in case that there is no such macro with that name. */

int isExistingMacro(MacroNode * mt, char * macro_name, int macroCount) {
    int i;

    for(i = 0 ; i < macroCount ; i++) { /* Loop through the macros array and if you found a macro with that name, return it's position in the table */
        if(!strcmp(mt[i].name, macro_name))
            return i;
    }

    return NOT_IN_MACRO_TABLE;
}

/* This function recieves a string (we pass lines to it) and returns true if that string signifies a macro ending
(basically, wether that string is "endmcr", and it may contain additional trailing spaces as well). */

bool macroEnding(char * str) {
    char word[MAX_LINE_LENGTH];

    getWord(word, MAX_LINE_LENGTH, &str);

    if(!strcmp(word, "endmcr") && isEmptyLine(str))
        return true;

    return false;
}

/* This function recieves a string that signifies a macro definition, extract the desired macro's name from that string
and returns true if the desired macro's name is legal, and false otherwise. */

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

/* This function recieves a macro pointer and a string to add to this macro's definition, and adds that string to that macro's definition. */

void addToMacroDefinition(MacroNode * macro, char * add) {
    int currentLen = strlen(macro->definition); /* Current length of macro definition */
    int addLen = strlen(add); /* The length of the string we wish to add */

    if( (macro->size) - currentLen < (addLen + 1) ) { /* If the current macro's size isn't enough to contain it's content + the new string + an '\0' character */
        macro->definition = my_realloc(macro->definition, macro->size + addLen + 1); /* enlrage that macro's size just to be enough to contain all that */
        macro->size += addLen + 1; /* change macro->size to signify the new size after the addition of space */
    }
    
    strcat(macro->definition, add); /* concatenate the string we wish to add to that macro's definition. */
}