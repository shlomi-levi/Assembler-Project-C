#include "../headers/headers.h"
#include "../headers/input.h"
#include "../headers/commands.h"
#include "../headers/labels.h"

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

LineClassification classifyLine(char * line, bool isSecondRun) {
    LineClassification lc = createEmptyLineClassification();

    char returnCommand[MAX_LINE_LENGTH];
    char word[MAX_LINE_LENGTH];
    
    int colCounter = 0;
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
    
    getWord(word, MAX_LINE_LENGTH, &line);

    colCounter = countOccurences(word, ':');
    
    if(colCounter > 1) {
        lc.type = LINE_INVALID;
        strcpy(lc.errorMessage, "Only one colon is allowed (and that is, when declaring a label).");
        return lc;
    }

    if(colCounter == 1) {
        int colonLocation = contains(word, ':');
        word[colonLocation] = '\0';
        
        if(!isValidLabelName(word)) {
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

        word[colonLocation] = ' ';

        getWord(word, MAX_LINE_LENGTH, &line);

        hasLabel = true;
    }

    if(!strcmp(word, ".extern") || !strcmp(word, ".entry")) {

        if(!strcmp(word, ".extern")) {
            lc.type = LINE_EXTERN;

            if(hasLabel)
                lc.labelForExtern = true;
        }
        
        else {
            lc.type = LINE_ENTRY;

            if(hasLabel)
                lc.labelForEntry = true;
        }
        
        if(!isSecondRun)
            handleExternOrEntry(&lc, line);

        return lc;
    }

    if(!strcmp(word, ".data")) {
        lc.type = LINE_DATA;

        if(!isSecondRun)
            readData(&lc, line);

        return lc;
    }

    if(!strcmp(word, ".string")) {
        lc.type = LINE_DATA;
        
        if(!isSecondRun)
            readString(&lc, line);
        return lc;
    }

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

void readData(LineClassification * lc, char * line) {
    int count = 0;
    int i = 0;
    int * dataArray = NULL;
    int len;
    
    bool comma = false; /* Is comma permitted at this stage */
    bool sign = true; /* Is sign permitted at this stage */
    bool digit = true;
    bool numberInProgress = true;

    bool holdsData = false;

    char currentNum[MAX_LINE_LENGTH];
    char charArray[2];
    char c;

    removeSpacesFromStart(line);
    
    len = strlen(line);

    charArray[1] = '\0';

    memset(currentNum, 0, MAX_LINE_LENGTH);

    for(i = 0 ; i < len ; i++) {
        c = line[i];

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

            count++;

            dataArray = (holdsData) ? realloc(dataArray, count * sizeof(int)) : malloc(sizeof(int));

            holdsData = true;

            dataArray[count - 1] = atoi(currentNum);

            memset(currentNum, 0, MAX_LINE_LENGTH);

            numberInProgress = false;

            sign = true;
        }

    }

    if(!comma || !count) {
        lc->type = LINE_INVALID;

        if(!count)
            strcpy(lc->errorMessage, "Data should contain at least one integer");
        else
            strcpy(lc->errorMessage, "Incorrect data format");

        return;
    }

    if(numberInProgress) {
        count++;

        dataArray = (holdsData) ? my_realloc(dataArray, count * sizeof(int)) : my_malloc(sizeof(int));
        
        dataArray[count - 1] = atoi(currentNum);
    }

    lc->data = dataArray;
    lc->dataLength = count;

    lc->type = LINE_DATA; /* Should already be on that value, but just in case. */
}

void readString(LineClassification * lc, char * line) {
    int i;
    int start = DOESNT_CONTAIN;
    int end = DOESNT_CONTAIN;

    char c = '"';

    int len = strlen(line);
    int outputLen;

    /* Find first " */

    for(i = 0 ; i < len ; i++) {
        if(line[i] != c) {
            if(isspace(line[i]))
                continue;
            
            lc->type = LINE_INVALID;
            strcpy(lc->errorMessage, "Invalid string format. string should start with \"");
            return;
        }
        start = i;
        break;
    }

    if(start == DOESNT_CONTAIN || start == len - 1) {
        lc->type = LINE_INVALID;
        strcpy(lc->errorMessage, "Invalid string format. string should include at least two apostrophes");
        return;
    }


    /* Find last " */
    for(i = len - 1 ; i >= 0 ; i--) {
        if(line[i] != c) {
            if(isspace(line[i]))
                continue;
            
            lc->type = LINE_INVALID;
            strcpy(lc->errorMessage, "Invalid string format. string must end with \"");
            return;
        }

        end = i;
        break;
    }

    if(start == end || end == (start + 1)) {
        lc->type = LINE_INVALID;
        
        if(start == end)
            strcpy(lc->errorMessage, "Invalid string format. string should include at least two apostrophes");

        else
            strcpy(lc->errorMessage, "Invalid string - string cannot be empty.");

        return;
    }

    lc->type = LINE_DATA;

    start++; /* Location of first character to copy */
    end--; /* Location of last character to copy */

    line += start;

    outputLen = end - start + 1;

    lc->data = my_calloc(outputLen + 1, sizeof(int));
    lc->dataLength = outputLen + 1;

    for(i = 0 ; i < outputLen; i++) {
        int d = line[i];
        lc->data[i] = d;
    }

    lc->data[lc->dataLength - 1] = 0;
}

void handleExternOrEntry(LineClassification * lc, char * line) {
    char word[MAX_LABEL_LENGTH];

    getWord(word, MAX_LABEL_LENGTH, &line);

    if(!isValidLabelName(word)) {
        lc->type = LINE_INVALID;
        strcpy(lc->errorMessage, "Illegal label name");
        return;
    }

    strcpy(lc->label, word);

    if(!isEmptyLine(line)) {
        lc->type = LINE_INVALID;
        strcpy(lc->errorMessage, "Only white spaces should be allowed after the label name");
        return;
    }
}