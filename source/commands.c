#include "../headers/headers.h"
#include "../headers/labels.h"
#include "../headers/commands.h"

/* An array that holds data about commands,
The commands name, number of parameters, and for each parameter the legal addressing types. */

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
                /* If the command has 0 arguments, then after the command name we only allow spaces and tabs */

                if(!isEmptyLine(line)) {
                    output.validCommand = false;
                    strcpy(output.errorMessage, "Invalid usage of command. This command has no parameters");
                    break;
                }
                break;
            }

            case 1: {
                getFirstArgument(word, &line); /* This is the dest parameter since this command has only 1 paraemters */
                
                /* Check that the parameter isn't an empty line */

                if(isEmptyLine(word)) {
                    strcpy(output.errorMessage, "Invalid number of arguments. This command needs to have 1 argument.");
                    output.validCommand = false;
                    break;                
                }

                handleArgument(word, &output, cmd.legalDestAddressing, number, 1);

                if(!output.validCommand)
                    break;

                /* If the command has 1 arguments, then after the dest parameter we only allow spaces and tabs */

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

    /* Verify that the argument has a legal addressing mode */
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

/* This function recieves an immediate (#number) and parses the number to ret.
If the argument is illegal, It changes the appropriate fields in output */

void parseImmediate(char * ret, char * word, commandOutput * output) {
    int num;
    int numLen;
    int start = 0;
    int end;

    bool negative = false;

    char number[MAX_NUMBER_LENGTH];

    if(word[0] != '#') {
        output->validCommand = false;
        strcpy(output->errorMessage, "Immediate value should start with '#'");
        return;
    }

    word++; /* get rid of '#' char */

    /* If the word doesn't represent a number */
    if(!isNumber(word)) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid immediate value");
        return;
    }

    /* We allow '+' and '-' at the very beginning */
    if(word[0] == '-' || word[0] == '+') {
        start++;

        if(word[0] == '-')
            negative = true;
    }

    end = strlen(word);

    /* If the number starts with zeros, we don't care about them */
    while(start < end - 1) {
        if(word[start] != '0')
            break;

        start++;
    }

    word+= start;
    numLen = strlen(word);

    if(numLen > MAX_CHARS_IN_IMMEDIATE) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Immediate value must be in the range of 12 bits");
        return;
    }

    num = atoi(word);

    if(negative)
        num *= -1;

    if(num < MIN_VALUE_FOR_LABEL_OR_IMMEDIATE || num > MAX_VALUE_FOR_LABEL_OR_IMMEDIATE) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Immediate value must be in the range of 12 bits");
        return;
    }

    memset(number, 0, MAX_NUMBER_LENGTH);

    if(negative)
        strcat(number, "-");

    strcat(number, word);

    strcpy(ret, number);
}

/* This function handles jump commands.
A jump command should have a label name after the command's name, and can have up to two additional parameters in parenthesis after the label's name.
The function recives a line pointer after the part of the jump command's name.
If there are additional arguments (between 1-2) then they appear in parenthesis seperated with a comma (if theres two) and no spaces
(neither between the jump parameter and the '(' character, and not inside the parenthesis themselves). */

void handleJumpCommand(commandOutput * output, COMMANDS_INFO cmd, char * line) {
    char word[MAX_LINE_LENGTH];
    char jumpDestination[MAX_LINE_LENGTH]; /* For the jump parameter */
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

    removeRedundantSpaces(line); /* Get rid of spaces */

    /* In the case of jump commands, these two fields will refer to the two additional optional parameters. */
    output->srcAddressing = addressingMode_IRRELEVANT;
    output->destAddressing = addressingMode_IRRELEVANT;

    getWord(word, MAX_LINE_LENGTH, &line); /* This is already after we have removed the command's name from the line */

    if(!isEmptyLine(line)) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Jump command with no destination parameter");
        return;
    }

    leftBrace = contains(word, '('); /* Get the position of the first left brace */
    rightBrace = contains(word, ')'); /* Get the position of the first right brace */

    /* If we have a left brace and dont have a right brace (or vice versa) the input is invalid */
    if( (leftBrace == DOESNT_CONTAIN && rightBrace != DOESNT_CONTAIN) || 
    (leftBrace != DOESNT_CONTAIN && rightBrace == DOESNT_CONTAIN) ) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid usage of parenthesis");
        return;
    }

    /* Now the string either contains both left brace and right brace, or neither of them */

    /* If it doesnt contain neither: */
    if(leftBrace == DOESNT_CONTAIN) {
        /* in this case we have no parameters */
        am = getAddressingMode(word);

        if(am == addressingMode_INVALID || !validAddressingMode(am, cmd.legalDestAddressing)) {
            output->validCommand = false;
            strcpy(output->errorMessage, "Invalid addressing mode for jump parameter");
            return;

        }

        strcpy(output->jumpLabel, word); /* Set command output's jump label */

        return;
    }

    /* If we have a left brace and a right brace */

    /* We need to make sure that we have exactly one left '(' and one ')' */
    if(countOccurences(word, '(') > 1) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid usage of parenthesis. too many left parenthesis (max is 1)");
        return;
    }

    if(countOccurences(word, ')') > 1) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid usage of parenthesis. too many right parenthesis (max is 1)");
        return;
    }

    /* We also need to make sure that '(' appears before ')' */
    if(leftBrace != DOESNT_CONTAIN && rightBrace != DOESNT_CONTAIN && leftBrace > rightBrace) {
        output->validCommand = false;
        strcpy(output->errorMessage,  "Invalid usage of parenthesis - left parenthesis should appear before right parenthesis.");
        return;
    }
    
    /* If the position of the left brace is 0 (the first char in the array), that means we dont have a jump parameter */
    if(leftBrace == 0) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid label name");
        return;
    }

    memset(jumpDestination, 0, MAX_LINE_LENGTH);
    substring(jumpDestination, word, 0, leftBrace - 1); /* Get the jump parameter */

    am = getAddressingMode(jumpDestination); /* Get the jump parameter's addressing mode */

    if(!validAddressingMode(am, cmd.legalDestAddressing)) { /* Check its validity */
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid addressing mode for destination parameter");
        return;
    }

    strcpy(output->jumpLabel, jumpDestination);
    
    /* Check if there is anything besides whitespace after the right parenthesis */
    if(!isEmptyLine(line + rightBrace + 1)) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid usage of command - there shouldn't be anything after the right parenthesis");
        return;
    }

    /* Check if there's more than one comma */
    if(countOccurences(word, ',') > 1) {
        output->validCommand = false;
        strcpy(output->errorMessage, "Invalid number of commas, should be 1 at most.");
        return;
    }

    comma = contains(word, ','); /* Get the comma's position */

    if(comma == DOESNT_CONTAIN) /* If we dont have a comma, that means we have only 1 additional argument */
        substring(srcParam, word, leftBrace + 1, rightBrace - 1);

    else { /* If we do have a comma, that means we have two additional arguments */
        if(comma < leftBrace || comma > rightBrace) { /* The comma must be located between the left brace and the right brace */
            output->validCommand = false;
            strcpy(output->errorMessage, "Comma should be found between the left parenthesis and the right parenthesis");
            return;
        }
        substring(srcParam, word, leftBrace + 1, comma - 1);
        substring(destParam, word, comma + 1, rightBrace - 1);
    }

    /* Handle first additional argument */
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

    /* Before handling the second additional argument we first need to check if there is such */

    if(destParam[0] != '\0') { /* We do it by checking if the destParam string starts with a null or not. */
        /* Handle second additional argument */
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

    /* Increase the number of words that this command has, according to its additional arguments */
    if(output->srcAddressing == addressingMode_REGISTER_IMMEDIATE && output->destAddressing == addressingMode_REGISTER_IMMEDIATE)
        output->numOfWords++;
    
    else {
        if(output->srcAddressing != addressingMode_IRRELEVANT)
            output->numOfWords++;

        if(output->destAddressing != addressingMode_IRRELEVANT)
            output->numOfWords++;
    }
}

/* A function that recieves a command's argument as a parameter and return this argument's addressing mode. */

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

    /* Handle immediate register addressing mode */
    for(i = 0 ; i < NUM_OF_REGISTERS ; i++) {
        if(!strcmp(registersNames[i], argument))
            return addressingMode_REGISTER_IMMEDIATE;
    }

    /* The only addressing mode that is left is direct addressing mode. meaning that we need to check if the argument is a valid label name. */
    
    if(!isValidLabelName(argument))
        return addressingMode_INVALID;

    return addressingMode_DIRECT;
}

/* A function that recieves a output pointer and a pointer to a line pointer as a parameter and gets the second argument of that command
and places it in the output char array
It is guarenteed that *line points to a command line after the command name */

void getFirstArgument(char * out, char ** line) {
    int len;

    getWord(out, MAX_LINE_LENGTH, line);

    len = strlen(out);
    
    if(out[len - 1] == ',') /* If we have a comma at the end we dont need it. */
        out[len - 1] = '\0';
}

/* A function that recieves a output pointer and a pointer to a line pointer as a parameter and gets the second argument of that command
and places it in the output char array
It is guarenteed that *line points to a command line after the first parameter of the command */

void getSecondArgument(char * output, char ** line) {
    int len;

    getWord(output, MAX_LINE_LENGTH, line); /* Try to get the second parameter */

    if(!strcmp(output, ",")) { /* If you got ',' get the next word (that is guarenteed to be the second parameter) */
        getWord(output, MAX_LINE_LENGTH, line);
        return;
    }
    
    /* for cases where we got ',' and the parameter right after (meaning that the parameter is not seperated from the comma with at least one space)*/
    if(output[0] == ',') {
        /* Get rid of the first character of the string by using a helper array */
        char helper[MAX_LINE_LENGTH];
        memset(helper, 0, MAX_LINE_LENGTH);

        len = strlen(output);

        memcpy(helper, output + 1, len - 1); /* Copy from output[1] to output[len - 1], to get rid of the comma */
        strcpy(output, helper); /* Copy helper to output */
    }
}

/* A function that recieves a string as a parameter and returns true if there's a command with that name */

bool isCommand(char * input) {
    int i;

    for(i = 0 ; i < NUM_OF_COMMANDS ; i++) {
        if(!strcmp(commands[i].name, input))
            return true;
    }
    return false;
}

/* A function that recieves a word as a parameter and returns the command that this word represents.
This function already assumes that the parameter it is given is a function. */

COMMANDS_INFO findCommand(char * word) {
    int i;

    for(i = 0 ; i < NUM_OF_COMMANDS ; i++) {
        if(!strcmp(commands[i].name, word))
            return commands[i];
    }
    return commands[0]; /* We won't get here anyway but we need to return some value at the end */
}

/* A function that recieves an addressing mode and a struct of legal addressing modes, and returns true if the addressing mode is a legal addressing mode in that struct */

bool validAddressingMode(enum addressingModes am, AddressingStruct str) {

    if(am == addressingMode_INVALID)
        return false;
    
    /* Basically just return the appropriate struct field */
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

/* A function that creates an empty command output */

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

/* A function that recieves a command type as a parameter and returns true if that command is a jump command, false otherwise. */

bool isJumpCommand(enum commandTypes cmdType) {
    if(cmdType == BNE || cmdType == JSR || cmdType == JMP)
        return true;

    return false;
}

/* A function that recieves a line as a parameter and returns true if that line is a comment, false otherwise. */

bool isComment(char * line) {
    return (line[0] == ';') ? true : false;
}