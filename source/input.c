/* This file is responsible for lines classification and everything input related (except commands). */

#include "../headers/headers.h"
#include "../headers/input.h"
#include "../headers/commands.h"
#include "../headers/labels.h"

/* This function creates an empty line classification */

LineClassification createEmptyLineClassification() {
    LineClassification lc;

    lc.type = LINE_INVALID;
    lc.data = NULL;
    lc.dataLength = 0;
    lc.labelForExtern = false;
    lc.labelForEntry = false;

    memset(lc.label, 0, MAX_LABEL_LENGTH);
    memset(lc.commandWithoutLabel, 0, MAX_LINE_LENGTH);
    memset(lc.errorMessage, 0, MAX_LINE_LENGTH);

    return lc;
}

/* This function classifies a line */

LineClassification classifyLine(char * line, bool isSecondRun) {
    LineClassification lc = createEmptyLineClassification();

    char returnCommand[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    
    int colCounter = 0; /* for counting colons */
    bool hasLabel = false;

    memset(returnCommand, 0, MAX_LINE_LENGTH);

    if(isEmptyLine(line)) {
        lc.type = LINE_EMPTY;
        return lc;
    }

    if(line[0] == ';') {
        lc.type = LINE_COMMENT;
        return lc;
    }
    
    getWord(word, MAX_LINE_LENGTH, &line); /* Get the first word of the line */

    colCounter = countOccurences(word, ':'); /* count the number of colons */
    
    if(colCounter > 1) { /* If there's more than one colon, the line is invalid. */
        lc.type = LINE_INVALID;
        strcpy(lc.errorMessage, "Only one colon is allowed (and that is, when declaring a label).");
        return lc;
    }

    if(colCounter == 1) { /* If there is one colon */
        int colonLocation = contains(word, ':'); /* Find its position*/
        word[colonLocation] = '\0'; /* Temporarily place a null character in that position so we could treat what comes before as a seperate string */
        
        if(!isValidLabelName(word)) { /* If what comes before that colon isn't a valid label name, the line is invalid.*/
            lc.type = LINE_INVALID;
            strcpy(lc.errorMessage, "Illegal label name - Label name must start with a letter, and contain only alphanumeric characters.");
            return lc;
        }

        /* If trying to decalre a label name using a saved word */
        if(isCommand(word)) {
            lc.type = LINE_INVALID;
            strcpy(lc.errorMessage, "Trying to use a command as a label name is forbidden");
            return lc;
        }

        strcpy(lc.label, word);

        /* Place a space instead of the colon so that in case there wasn't a space between the colon and the next word,
        now there is, and we can parse the string using the function getWord. */
        word[colonLocation] = ' ';

        getWord(word, MAX_LINE_LENGTH, &line); /* get the next word */

        hasLabel = true;
    }

    /* Handle .extern or .entry lines */
    if(!strcmp(word, ".extern") || !strcmp(word, ".entry")) {

        if(!strcmp(word, ".extern")) {
            lc.type = LINE_EXTERN;

            if(hasLabel)
                lc.labelForExtern = true; /* So that we woud print a warning */
        }
        
        else {
            lc.type = LINE_ENTRY;

            if(hasLabel)
                lc.labelForEntry = true; /* So that we woud print a warning */
        }
        
        if(!isSecondRun) /* If this isn't the second run */
            handleExternOrEntry(&lc, line); /* Further handle .extern or .entry line*/

        return lc;
    }

    /* Handle .data lines */
    if(!strcmp(word, ".data")) {
        lc.type = LINE_VARIABLE;

        if(!isSecondRun) /* If this isn't the second run */
            readData(&lc, line); /* Read the data */

        return lc;
    }

    /* Handle .string lines */
    if(!strcmp(word, ".string")) {
        lc.type = LINE_VARIABLE;
        
        if(!isSecondRun) /* If this isn't the second run */
            readString(&lc, line); /* Read the string */
        return lc;
    }

    /* If we have reached thus far, the line should be a command since there are no other options left. */

    if(!isCommand(word)) {
        lc.type = LINE_INVALID;
        strcpy(lc.errorMessage, "Illegal command name.");
        return lc;
    }

    strcat(returnCommand, word);
    strcat(returnCommand, " ");
    strcat(returnCommand, line);

    strcpy(lc.commandWithoutLabel, returnCommand);
    lc.type = LINE_COMMAND;
    return lc;
}

/* This function reads a .data line,
It recieves a pointer to a .data variable declaration, after the .data part. */

void readData(LineClassification * lc, char * line) {
    int count = 0; /* Number of integers we've read */
    int i = 0;
    int * dataArray = NULL;
    int len; /* For the length of the string */
    
    bool comma = false; /* Is comma permitted at this stage */
    bool sign = true; /* Is sign permitted at this stage */
    bool digit = true; /* Is digit permitted at this stage */
    bool numberInProgress = true; /* Are we currently parsing a number */

    bool holdsData = false;

    char currentNum[MAX_LINE_LENGTH];
    char charArray[2];
    char c;

    /* Get rid of spaces from the start of the string and the end of the string. */
    removeRedundantSpaces(line);
    
    len = strlen(line);

    charArray[1] = '\0';

    memset(currentNum, 0, MAX_LINE_LENGTH);

    /* Loop through the string */
    for(i = 0 ; i < len ; i++) {
        c = line[i]; /* Get the current character */

        /* Verify validity */

        if(c != ' ' && c != ',' && !isdigit(c) && c != '+' && c != '-') {
            lc->type = LINE_INVALID;
            strcpy(lc->errorMessage, "Incorrect data format");
            return;
        }

        if( (!comma && c == ',') || (!sign && c == '+') || (!sign && c == '-') || (!digit && isdigit(c))) {
            lc->type = LINE_INVALID;
            strcpy(lc->errorMessage, "Incorrect data format");
            return;
        }

        if(isdigit(c) || (sign && c == '+') || (sign && c == '-')) {
            charArray[0] = c;
            strcat(currentNum, charArray);
            
            if(isdigit(c)) {
                comma = true;
                numberInProgress = true;
            }

            sign = false;
        }
        
        else if(c == ',' || c == ' ') {
            if(c == ',')
                comma = false;

            if(!numberInProgress)
                continue;

            /* If we reach thus far and we've encountered a comma or a whitespace, the current number is over, we need to add it to the array */
            
            count++;

            dataArray = (holdsData) ? realloc(dataArray, count * sizeof(int)) : malloc(sizeof(int));

            holdsData = true;

            dataArray[count - 1] = atoi(currentNum);

            memset(currentNum, 0, MAX_LINE_LENGTH);

            numberInProgress = false;

            sign = true;
        }

    }

    /* If the string ends with a number and no space */

    if(numberInProgress) {
        count++;

        dataArray = (holdsData) ? my_realloc(dataArray, count * sizeof(int)) : my_malloc(sizeof(int));
        
        dataArray[count - 1] = atoi(currentNum);
    }
    
    if(!comma || !count) {
        lc->type = LINE_INVALID;

        if(!count)
            strcpy(lc->errorMessage, "Data should contain at least one integer"); /* If we haven't reaad at least one integer in the array */
        else
            strcpy(lc->errorMessage, "Incorrect data format"); /* If the string ends with ',' */

        return;
    }

    lc->data = dataArray;
    lc->dataLength = count;
}

/* This function reads a string from .string line */
void readString(LineClassification * lc, char * line) {
    int i;
    int j;

    char c = '"';

    int len;
    int outputLen;

    /* Remove spaces from start and end */
    removeRedundantSpaces(line);

    len = strlen(line);

    /* Check for all sorts of errors that could occur */

    if(len < 2) {
        lc->type = LINE_INVALID;
        strcpy(lc->errorMessage, "Invalid string declaration - string must start with \" and end with \"");
        return;
    }

    if(line[0] != c) {
        lc->type = LINE_INVALID;
        strcpy(lc->errorMessage, "Invalid string declaration - string must start with \"");
        return;
    }

    if(line[len - 1] != c) {
        lc->type = LINE_INVALID;
        strcpy(lc->errorMessage, "Invalid string declaration - string must end with \"");
        return;
    }

    lc->type = LINE_VARIABLE;

    outputLen = len - 2 + 1; /* Explanation for the calculation - the output's length is the length of the string, - 2 apostrophes, + null character. */

    lc->data = my_calloc(outputLen, sizeof(int)); /* Allocate memory for the data */
    lc->dataLength = outputLen;

    /* Place the characters ascii codes inside the string array */
    for(i = 1, j = 0 ; i < len - 1 && j < outputLen ; i++, j++) {
        int d = line[i];
        lc->data[j] = d;
    }

    lc->data[outputLen - 1] = 0; /* Since every string ends with '\0' = 0 */
}

/* This function further verifies that an .extern or .entry line is valid,
The input is an .entry or .extern line (without a preceding label). */

void handleExternOrEntry(LineClassification * lc, char * line) {
    char word[MAX_LABEL_LENGTH];

    getWord(word, MAX_LABEL_LENGTH, &line); /* Get the desired extern/entry label's name */

    /* Verify that it is a valid label name */
    if(!isValidLabelName(word)) {
        lc->type = LINE_INVALID;
        strcpy(lc->errorMessage, "Illegal label name");
        return;
    }

    strcpy(lc->label, word);

    /* If there's something after the label, the line is invalid. */
    if(!isEmptyLine(line)) {
        lc->type = LINE_INVALID;
        strcpy(lc->errorMessage, "Only white spaces should be allowed after the label name");
        return;
    }
}