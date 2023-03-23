#include "../headers/headers.h"
#include "../headers/labels.h"
#include "../headers/commands.h"

/* An array that holds data about commands */
static const COMMANDS_INFO commands [NUM_OF_COMMANDS] = {
        { "mov", MOV, 2, {true, true, true}, {false, true, true}},
        { "cmp", CMP, 2, {true, true, true}, {true, true, true}},
        { "add", ADD, 2, {true, true, true}, {false, true, true}},
        { "sub", SUB, 2, {true, true, true}, {false, true, true}},
        { "lea", LEA, 2, {false, true, false}, {false, true, true}},
        { "not", NOT, 1, {false, false, false}, {false, true, true}},
        { "clr", CLR, 1, {false, false, false}, {false, true, true}},
        { "inc", INC, 1, {false, false, false}, {false, true, true}},
        { "dec", DEC, 1, {false, false, false}, {false, true, true}},
        { "jmp", JMP, 1, {false, false, false}, {false, true, true}},
        { "bne", BNE, 1, {false, false, false}, {false, true, true}},
        { "red", RED, 1, {false, false, false}, {false, true, true}},
        { "prn", PRN, 1, {false, false, false}, {true, true, true}},
        { "jsr", JSR, 1, {false, false, false}, {false, true, true}},
        { "rts", RTS, 0, {false, false, false}, {false, false, false}},
        { "stop", STOP, 0, {false, false, false}, {false, false, false}}
};

/* This function breaks down a command to its parameters, and checks the validity of them according to the
    allowed addressing modes */

commandOutput parseCommand(char * line) {
    commandOutput output;

    char word[MAX_LINE_LENGTH];

    char outputWord[MAX_OUTPUT_WORD];
    char number[MAX_LINE_LENGTH];
    COMMANDS_INFO cmd;

    enum addressingModes mode;

    output = createEmptyCommandOutput();

    memset(outputWord, 0, MAX_OUTPUT_WORD);

    getWord(word, MAX_LINE_LENGTH, &line); /* Firstly, get the commands name */

    /* If no command by this name, exists, this isn't a valid command. */
    if(!isCommand(word)) {
        output.validCommand = false;
        strcpy(output.errorMessage, "Invalid command");
        return output;
    }

    cmd = findCommand(word); /* Find the command that has that name */
    
    output.commandType = cmd.commandType;
    output.numOfWords = 1;

    if(isJumpCommand(cmd.commandType)) {
        output.numOfWords++; /* Now we have a word for the jump parameter */
        handleJumpCommand(&output, cmd, line); /* Handle Jump command */
        return output;
    }
    
    else {
        switch(cmd.numOfArguments) {
            case 0: {
                if(!isEmptyLine(line)) {
                    output.validCommand = false;
                    strcpy(output.errorMessage, "Invalid usage of command. This command has no parameters");
                    break;
                }
                break;
            }

            case 1: {
                getFirstArgument(word, &line); /* This is the dest parameter since this command has only 1 paraemters */
                
                if(isEmptyLine(word)) {
                    strcpy(output.errorMessage, "Invalid number of arguments. This command need to have 1 argument.");
                    output.validCommand = false;
                    break;                
                }

                handleArgument(word, &output, cmd.legalDestAddressing, number, 1);

                if(!output.validCommand)
                    break;

                if(!isEmptyLine(line)) /* After we have read the first parameter, we need to make sure that the line is empty */ {
                    strcpy(output.errorMessage, "Invalid usage of command. This command has only 1 parameter");
                    output.validCommand = false;
                    break;
                }

                mode = getAddressingMode(word);

                output.destAddressing = mode;
                
                if(mode == addressingMode_IMMEDIATE)
                    strcpy(output.destParameter, number);
                
                else
                    strcpy(output.destParameter, word);

                break;
            }

            case 2: {
                getFirstArgument(word, &line);

                if(isEmptyLine(word)) {
                    strcpy(output.errorMessage, "Invalid number of arguments. This command should have 2 arguments.");
                    output.validCommand = false;
                    break;
                }

                handleArgument(word, &output, cmd.legalSrcAddressing, number, 1);

                if(!output.validCommand)
                    break;

                mode = getAddressingMode(word);

                output.srcAddressing = mode;

                if(mode == addressingMode_IMMEDIATE)
                    strcpy(output.srcParameter, number);

                else
                    strcpy(output.srcParameter, word);

                getSecondArgument(word, &line);

                if(isEmptyLine(word)) {
                    strcpy(output.errorMessage, "Invalid number of arguments. This command should have 2 arguments.");
                    output.validCommand = false;
                    break;
                }

                handleArgument(word, &output, cmd.legalDestAddressing, number, 2);

                if(!output.validCommand)
                    break;
                
                mode = getAddressingMode(word);

                output.destAddressing = mode;

                if(mode == addressingMode_IMMEDIATE)
                    strcpy(output.destParameter, number);

                else
                    strcpy(output.destParameter, word);

                if(!isEmptyLine(line)) {
                    strcpy(output.errorMessage, "Invalid number of arguments");
                    output.validCommand = false;
                    break;
                }
                
                
                break;
            }
        }
    }

    if(!output.validCommand)
        return output;

    /* Now to calculate the number of words needed for this command */

    if(output.srcAddressing == addressingMode_REGISTER_IMMEDIATE && output.destAddressing == addressingMode_REGISTER_IMMEDIATE)
        output.numOfWords++;
    
    else {
        if(output.srcAddressing != addressingMode_IRRELEVANT)
            output.numOfWords++;

        if(output.destAddressing != addressingMode_IRRELEVANT)
            output.numOfWords++;

    }
    return output;
}

/* This function handles an argument - it checks wether its addressing mode is legal, 
and if that argument is an immediate, it makes sure it is in the range of 12 bits. */

void handleArgument(char * word, commandOutput * output, AddressingStruct legalAddressing, char * number, int argNumber) {
    enum addressingModes mode = getAddressingMode(word);

    if(!validAddressingMode(mode, legalAddressing)) {
        sprintf(output->errorMessage, "Invalid addressing mode for argument number (%d)", argNumber);
        output->validCommand = false;
        return;
    }
    
    /* If the parameter is an immediate, we need to check that it is within the range of 12 bits */
    if(mode == addressingMode_IMMEDIATE) {
        memset(number, 0, MAX_LINE_LENGTH);
        parseImmediate(number, word, output);

        if(!output->validCommand)
            return;
    }
}

void parseImmediate(char * ret, char * word, commandOutput * output) {
    int num;
    int numLen;
    int start = 0;
    int end;

    bool negative = false;

    char number[MAX_NUMBER_LENGTH];

    word++; /* get rid of '#' sign */    

    if(!isNumber(word)) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid immediate value");
        return;
    }

    if(word[0] == '-' || word[0] == '+') {
        start++;

        if(word[0] == '-')
            negative = true;
    }

    end = strlen(word);

    while(start < end - 1) {
        if(word[start] != '0')
            break;

        start++;
    }

    word+= start;
    numLen = strlen(word);

    if(numLen > MAX_CHARS_IN_IMMEDIATE) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Immediate value must be in the range of 14 bits");
        return;
    }

    num = atoi(word);

    if(negative)
        num *= -1;

    if(num < MIN_VALUE_FOR_LABEL_OR_IMMEDIATE || num > MAX_VALUE_FOR_LABEL_OR_IMMEDIATE) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Immediate value must be in the range of 14 bits");
        return;
    }

    memset(number, 0, MAX_NUMBER_LENGTH);

    if(negative)
        strcat(number, "-");

    strcat(number, word);

    strcpy(ret, number);
}

void handleJumpCommand(commandOutput * output, COMMANDS_INFO cmd, char * line) {
    char word[MAX_LINE_LENGTH];
    char jumpDestination[MAX_LINE_LENGTH];
    char srcParam[MAX_LINE_LENGTH];
    char destParam[MAX_LINE_LENGTH];

    int leftBrace;
    int rightBrace;
    int comma;
    enum addressingModes am;

    memset(word, 0, MAX_LINE_LENGTH);
    memset(jumpDestination, 0, MAX_LINE_LENGTH);
    memset(srcParam, 0, MAX_LINE_LENGTH);
    memset(destParam, 0, MAX_LINE_LENGTH);

    removeRedundantSpaces(line);

    output->srcAddressing = addressingMode_IRRELEVANT;
    output->destAddressing = addressingMode_IRRELEVANT;

    getWord(word, MAX_LINE_LENGTH, &line); /* This is already after we have removed the command's name from the line */

    if(!isEmptyLine(line)) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid number of arguments");
        return;
    }

    leftBrace = contains(word, '(');
    rightBrace = contains(word, ')');

    if( (leftBrace == DOESNT_CONTAIN && rightBrace != DOESNT_CONTAIN) || 
    (leftBrace != DOESNT_CONTAIN && rightBrace == DOESNT_CONTAIN) ) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid usage of parenthesis");
        return;
    }

    /* Now the string contains either contains both left brace and right brace, or neither of them */

    if(leftBrace == DOESNT_CONTAIN) {
        /* in this case we have no parameters */
        am = getAddressingMode(word);

        if(am == addressingMode_INVALID || !validAddressingMode(am, cmd.legalDestAddressing)) {
            output->validCommand = false;
            strcpy(output->errorMessage, "Invalid addressing mode for jump parameter");
            return;

        }

        strcpy(output->jumpLabel, word);

        return;
    }

    if(leftBrace != DOESNT_CONTAIN && rightBrace != DOESNT_CONTAIN && leftBrace > rightBrace) {
        output->validCommand = false;
        strcpy(output->errorMessage,  "Invalid usage of parenthesis - left parenthesis should appear before right parenthesis.");
        return;
    }

    if(leftBrace == 0) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid label name");
        return;
    }

    memset(jumpDestination, 0, MAX_LINE_LENGTH);
    substring(jumpDestination, word, 0, leftBrace - 1);

    am = getAddressingMode(jumpDestination);

    if(!validAddressingMode(am, cmd.legalDestAddressing)) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid addressing mode for destination parameter");
        return;
    }

    strcpy(output->jumpLabel, jumpDestination);

    if(countOccurences(word, '(') > 1 || countOccurences(word, ')') > 1) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid number of parenthesis");
        return;
    }
    
    /* Check if there is anything besides whitespace after the right parenthesis */
    if(!isEmptyLine(line + rightBrace + 1)) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid usage of command - there shouldn't be anything after the right parenthesis");
        return;
    }

    if(countOccurences(word, ',') > 1) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid number of commas, should be 1 at most.");
        return;
    }

    comma = contains(word, ',');

    if(comma == DOESNT_CONTAIN)
        substring(srcParam, word, leftBrace + 1, rightBrace - 1);

    else {
        if(comma < leftBrace || comma > rightBrace) {
            output->validCommand = false;
            strcpy(output->errorMessage, "Comma should be found between the left parenthesis and the right parenthesis");
            return;
        }
        substring(srcParam, word, leftBrace + 1, comma - 1);
        substring(destParam, word, comma + 1, rightBrace - 1);
    }

    am = getAddressingMode(srcParam);

    if(am == addressingMode_INVALID) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid addressing mode for argumet number 1 (inside the parenthesis)");
        return;
    }

    output->srcAddressing = am;

    if(am == addressingMode_IMMEDIATE) {
        parseImmediate(output->srcParameter, srcParam, output);

        if(!output->validCommand)
            return;
    }

    else
        strcpy(output->srcParameter, srcParam);

    if(destParam[0] != '\0') {
        am = getAddressingMode(destParam);
    
        if(am == addressingMode_INVALID) {
            output->validCommand = false;
            strcpy(output->errorMessage, "Invalid addressing mode for second parameter (inside the parenthesis)");
            return;
        }

        output->destAddressing = am;

        if(am == addressingMode_IMMEDIATE) {
            parseImmediate(output->destParameter, destParam, output);

            if(!output->validCommand)
                return;
        }

        else
            strcpy(output->destParameter, destParam);

        output->isJumpAddressing = true;
    }

    if(output->srcAddressing == addressingMode_REGISTER_IMMEDIATE && output->destAddressing == addressingMode_REGISTER_IMMEDIATE)
        output->numOfWords++;
    
    else {
        if(output->srcAddressing != addressingMode_IRRELEVANT)
            output->numOfWords++;

        if(output->destAddressing != addressingMode_IRRELEVANT)
            output->numOfWords++;
    }
}

enum addressingModes getAddressingMode(char * argument) {

    char * registersNames[NUM_OF_REGISTERS] = {
        "r0",
        "r1",
        "r2",
        "r3",
        "r4",
        "r5",
        "r6",
        "r7",
    };

    int len = strlen(argument);
    int i;

    /* Handle Immediate addressing mode */
    if(argument[0] == '#') {
        char num[MAX_LINE_LENGTH];

        if(len < 2)
            return addressingMode_INVALID;

        strcpy(num, argument + 1);

        if(!isNumber(num))
            return addressingMode_INVALID;
        
        return addressingMode_IMMEDIATE;
    }

    /* Handle direct register */
    for(i = 0 ; i < NUM_OF_REGISTERS ; i++) {
        if(!strcmp(registersNames[i], argument))
            return addressingMode_REGISTER_IMMEDIATE;
    }

    /* The only addressing mode that is left is direct addressing mode */
    
    if(!isValidLabelName(argument))
        return addressingMode_INVALID;

    return addressingMode_DIRECT;
}

void getFirstArgument(char *out, char ** line) {
    char argument[MAX_LINE_LENGTH];
    int wordLen;

    getWord(argument, MAX_LINE_LENGTH, line);

    wordLen = strlen(argument);
    
    if(argument[wordLen - 1] == ',')
        argument[wordLen - 1] = '\0';

    strcpy(out, argument);
}

void getSecondArgument(char * output, char ** line) {
    int len;
    int i;

    getWord(output, MAX_LINE_LENGTH, line);

    if(!strcmp(output, ",")) {
        getWord(output, MAX_LINE_LENGTH, line);
        return;
    }

    if(output[0] == ',') {
        len = strlen(output);

        for(i = 0 ; i < len ; i++)
            output[i] = output[i + 1];
    }
}

bool isCommand(char * input) {
    int i;

    for(i = 0 ; i < NUM_OF_COMMANDS ; i++) {
        if(!strcmp(commands[i].name, input))
            return true;
    }
    return false;
}

COMMANDS_INFO findCommand(char * word) {
    int i;

    for(i = 0 ; i < NUM_OF_COMMANDS ; i++) {
        if(!strcmp(commands[i].name, word))
            return commands[i];
    }
    return commands[0];
}

bool validAddressingMode(enum addressingModes am, AddressingStruct str) {

    if(am == addressingMode_INVALID)
        return false;
    
    switch(am) {
        case addressingMode_IMMEDIATE:
            return str.Addressing_IMMEDIATE;

        case addressingMode_DIRECT:
            return str.Addressing_DIRECT;

        case addressingMode_REGISTER_IMMEDIATE:
            return str.Addressing_REGISTER_IMMEDIATE;

        default: break;
    }

    return false;
}

commandOutput createEmptyCommandOutput() {
    commandOutput c;

    c.validCommand = true;
    c.isJumpAddressing = false;
    c.srcAddressing = addressingMode_IRRELEVANT;
    c.destAddressing = addressingMode_IRRELEVANT;
    c.isJumpAddressing = false;

    memset(c.srcParameter, 0, MAX_LINE_LENGTH);
    memset(c.destParameter, 0, MAX_LINE_LENGTH);
    memset(c.jumpLabel, 0, MAX_LABEL_LENGTH);
    
    return c;
}

bool isJumpCommand(enum commandTypes cmdType) {
    if(cmdType == BNE || cmdType == JSR || cmdType == JMP)
        return true;

    return false;
}

bool isComment(char * line) {
    return (line[0] == ';') ? true : false;
}