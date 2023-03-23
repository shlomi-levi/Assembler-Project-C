#include "../headers/headers.h"
#include "../headers/labels.h"
#include "../headers/commands.h"
#include "../headers/encoding.h"
#include "../headers/externals.h"

#define START_OF_BINARY_REPRESENTATION_IN_SRC_REGISTER 5
#define END_OF_BINARY_REPRESENTATION_IN_SRC_REGISTER 0

#define START_OF_BINARY_REPRESENTATION_IN_DST_REGISTER 11
#define END_OF_BINARY_REPRESENTATION_IN_DST_REGISTER 6

#define START_SRC START_OF_BINARY_REPRESENTATION_IN_SRC_REGISTER
#define END_SRC END_OF_BINARY_REPRESENTATION_IN_SRC_REGISTER

#define START_DST START_OF_BINARY_REPRESENTATION_IN_DST_REGISTER
#define END_DST END_OF_BINARY_REPRESENTATION_IN_DST_REGISTER

/* This function recieves a command as a parameter, and sets output to be the string that contains all of the words that this command generated (seperated with new lines). */

void getCommandEncoding(char * output, commandOutput cmd, LabelNode * labelArray, int labelArraySize, EntryNode ** externalsArray, int * externalsArraySize, int * wordCount) {
    char word[WORD_LENGTH];
    char temp[MAX_LINE_LENGTH]; /* A string that will contain all of the words this command generated, seperated with new lines.*/
    char encodingSrc[WORD_LENGTH];
    char encodingDest[WORD_LENGTH];

    int i;

    zeroBits(word, WORD_LENGTH);
    zeroBits(temp, MAX_LINE_LENGTH);
    zeroBits(encodingSrc, WORD_LENGTH);
    zeroBits(encodingDest, WORD_LENGTH);

    memset(output, 0, MAX_CHARS_PER_COMMAND);

    generateFirstWord(word, cmd); /* Generate the first word */

    convertToUniqueBase2(word); /* Convert it to base 2 */

    sprintf(temp, "%.4d\t%s\n", *wordCount, word); /* format temp */

    strcat(output, temp); /* concatenate temp to output*/

    (*wordCount)++; /* Increase word count */

    memset(word, 0, WORD_LENGTH);

    if(isJumpCommand(cmd.commandType)) {
        enum addressingModes am = getAddressingMode(cmd.jumpLabel);

        /* If the jump parameter is a label, we need to check if its an external label so that we could add it to the externals array if needed */
        if(am == addressingMode_DIRECT)
            checkForExternals(labelArray, labelArraySize, externalsArray, externalsArraySize, cmd.jumpLabel, *wordCount);

        getEncoding(word, am, cmd.jumpLabel, NONE, labelArray, labelArraySize); /* Get the jump param encoding */

        convertToUniqueBase2(word);

        sprintf(temp, "%.4d\t%s\n", *wordCount, word);
        
        strcat(output, temp);

        (*wordCount)++;
    }

    /* If there are source/dest parameters, get their encodings and convert them to unique base 2*/
    if(cmd.srcAddressing != addressingMode_IRRELEVANT)
        getEncoding(encodingSrc, cmd.srcAddressing, cmd.srcParameter, SOURCE_REGISTER, labelArray, labelArraySize);

    if(cmd.destAddressing != addressingMode_IRRELEVANT)
        getEncoding(encodingDest, cmd.destAddressing, cmd.destParameter, DESTINATION_REGISTER, labelArray, labelArraySize);
    
    convertToUniqueBase2(encodingSrc);
    convertToUniqueBase2(encodingDest);

    /* If both the source and destination parmeters are registers, they need to share a word. this part handles that.*/
    if(cmd.srcAddressing == addressingMode_REGISTER_IMMEDIATE && cmd.destAddressing == cmd.srcAddressing) {
        for(i = START_DST ; i >= END_DST ; i--) /* We copy the destination encoding bits to the source encoding array*/
            encodingSrc[i] = encodingDest[i];

        sprintf(temp, "%.4d\t%s\n", *wordCount, encodingSrc);
        strcat(output, temp);

        (*wordCount)++;

        return;
    }

    /* If this is not the case, we need to handle each parameter seperately */

    if(cmd.srcAddressing != addressingMode_IRRELEVANT) {

        if(cmd.srcAddressing == addressingMode_DIRECT)
            checkForExternals(labelArray, labelArraySize, externalsArray, externalsArraySize, cmd.srcParameter, *wordCount);

        sprintf(temp, "%.4d\t%s\n", *wordCount, encodingSrc);
        strcat(output, temp);

        (*wordCount)++;
    }

    if(cmd.destAddressing != addressingMode_IRRELEVANT) {

        if(cmd.destAddressing == addressingMode_DIRECT)
            checkForExternals(labelArray, labelArraySize, externalsArray, externalsArraySize, cmd.destParameter, *wordCount);
        
        sprintf(temp, "%.4d\t%s\n", *wordCount, encodingDest);
        strcat(output, temp);

        (*wordCount)++;
    }
}

/* This function recieves a parameter and an addressing mode, and places inside output the appropriate word (depending on the parameter's addressing method)
at the end output will have the approrpiate word in regular base 2. */

void getEncoding(char * output, enum addressingModes am, char * param, enum REGISTER_TYPE rt, LabelNode * labelArray, int labelArraySize) {
    int temp;
    char tempString[WORD_LENGTH];
    int i;
    int j;
    LabelNode l;

    memset(tempString, 0, WORD_LENGTH);
    zeroBits(output, WORD_LENGTH);

    /* Handle each addressing mode appropriately */
    switch(am) {
        case addressingMode_REGISTER_IMMEDIATE:
            putARE(output, ENCODING_ABSOLUTE);
            
            temp = getRegisterNumber(param);
            
            getBase2(tempString, temp, BITS_IN_REGISTER_REPRESENTATION);

            /* If it is the source register, the register's number bits are 8-13*/
            if(rt == SOURCE_REGISTER) {
                for(i = START_SRC, j = BITS_IN_REGISTER_REPRESENTATION - 1 ; i >= END_SRC ; i--, j--)
                    output[i] = tempString[j];
            }

            /* If its the destination register, the register's number bits are 2-7 */
            else if(rt == DESTINATION_REGISTER) {
                for(i = START_DST, j = BITS_IN_REGISTER_REPRESENTATION - 1 ; i >= END_DST ; i--, j--)
                    output[i] = tempString[j];
            }

            break;

            case addressingMode_IMMEDIATE:
                putARE(output, ENCODING_ABSOLUTE);
                temp = atoi(param);
                getBase2(tempString, temp, NUM_OF_BITS_WITHOUT_ARE);

                /* In the case of immediate, the 12 leftmost bits should represent the immediate's value */
                for(i = 0 ; i < NUM_OF_BITS_WITHOUT_ARE ; i++)
                    output[i] = tempString[i];
                
                break;
            
            /* Handle direct addressing mode */
            case addressingMode_DIRECT:
                l = findLabel(labelArray, labelArraySize, param);
                
                if(l.isExternal)
                    putARE(output, ENCODING_EXTERNAL);

                else
                    putARE(output, ENCODING_RELOCATABLE);

                getBase2(tempString, l.labelAddress, NUM_OF_BITS_WITHOUT_ARE);

                /* The 12 leftmost bits should be the label's address */
                for(i = 0 ; i < NUM_OF_BITS_WITHOUT_ARE ; i++)
                    output[i] = tempString[i];

                break;
            
            default: break;
    }
}

/* This function generates the first word of a command */

void generateFirstWord(char * output, commandOutput cmd) {
    char sourceAddressingStr[3];
    char destAddressingStr[3];
    char opcodeStr[5];

    /* For jump Addressing */
    char firstParameterAddressing[3];
    char secondParameterAddressing[3];

    zeroBits(sourceAddressingStr, 3);
    zeroBits(destAddressingStr, 3);
    zeroBits(opcodeStr, 5);

    zeroBits(firstParameterAddressing, 3);
    zeroBits(secondParameterAddressing, 3);

    zeroBits(output, WORD_LENGTH);

    if(isJumpCommand(cmd.commandType)) {
        enum addressingModes am = cmd.isJumpAddressing ? addressingMode_JUMP_ADDRESSING : getAddressingMode(cmd.jumpLabel);
        getBase2(destAddressingStr, am, 2);
    }

    else {
        if(cmd.srcAddressing != addressingMode_IRRELEVANT)
            getBase2(sourceAddressingStr, cmd.srcAddressing, 2);

        if(cmd.destAddressing != addressingMode_IRRELEVANT)
            getBase2(destAddressingStr, cmd.destAddressing, 2);
    }

    getBase2(opcodeStr, cmd.commandType, 4);

    /* Because the A.R.E bits of the first word are always 00 */
    output[13] = '0';
    output[12] = '0';

    /* Dest Addressing Type Bits */
    output[11] = destAddressingStr[1];
    output[10] = destAddressingStr[0];

    /* Source Addressing Type Bits */
    output[9] = sourceAddressingStr[1];
    output[8] = sourceAddressingStr[0];

    /* Op code bits */
    output[7] = opcodeStr[3];
    output[6] = opcodeStr[2];
    output[5] = opcodeStr[1];
    output[4] = opcodeStr[0];

    if(!cmd.isJumpAddressing)
        return;

    getBase2(firstParameterAddressing, cmd.srcAddressing, 2);
    getBase2(secondParameterAddressing, cmd.destAddressing, 2);

    output[3] = secondParameterAddressing[1];
    output[2] = secondParameterAddressing[0];

    output[1] = firstParameterAddressing[1];
    output[0] = firstParameterAddressing[0];
}

/* This function places A.R.E bits inside the output parameter, according to the encoding it is given as a parameter. */
void putARE(char * output, enum ENCODING e) {
    if(e == ENCODING_EXTERNAL) {
        output[12] = '0';
        output[13] = '1';
    }

    else if(e == ENCODING_RELOCATABLE) {
        output[12] = '1';
        output[13] = '0';
    }

    else if(e == ENCODING_ABSOLUTE) {
        output[12] = '0';
        output[13] = '0';
    }
}

/* This function gets a string that represents a register (like "r6") and returns the register's number. */
int getRegisterNumber(char * str) {
    char temp[2];
    temp[0] = str[1];
    temp[1] = '\0';

    return atoi(temp);
}