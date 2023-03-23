#include "../headers/headers.h"
#include "../headers/labels.h"
#include "../headers/commands.h"
#include "../headers/encoding.h"

#define START_OF_BINARY_REPRESENTATION_IN_SRC_REGISTER 5
#define END_OF_BINARY_REPRESENTATION_IN_SRC_REGISTER 0

#define START_OF_BINARY_REPRESENTATION_IN_DST_REGISTER 11
#define END_OF_BINARY_REPRESENTATION_IN_DST_REGISTER 6

#define START_SRC START_OF_BINARY_REPRESENTATION_IN_SRC_REGISTER
#define END_SRC END_OF_BINARY_REPRESENTATION_IN_SRC_REGISTER

#define START_DST START_OF_BINARY_REPRESENTATION_IN_DST_REGISTER
#define END_DST END_OF_BINARY_REPRESENTATION_IN_DST_REGISTER

void getCommandEncoding(char * output, commandOutput cmd, LabelNode * labelArray, int labelArraySize, int * wordCount) {
    char word[WORD_LENGTH];
    char temp[MAX_LINE_LENGTH];
    char encodingSrc[WORD_LENGTH];
    char encodingDest[WORD_LENGTH];

    int i;

    zeroBits(word, WORD_LENGTH);
    zeroBits(temp, MAX_LINE_LENGTH);
    zeroBits(encodingSrc, WORD_LENGTH);
    zeroBits(encodingDest, WORD_LENGTH);

    memset(output, 0, MAX_CHARS_PER_COMMAND);

    generateFirstWord(word, cmd);

    sprintf(temp, "%.4d\t%s\n", *wordCount, word);

    strcat(output, temp);

    (*wordCount)++;

    memset(word, 0, WORD_LENGTH);

    if(cmd.commandType == BNE || cmd.commandType == JMP || cmd.commandType == JSR) {
        enum addressingModes am = getAddressingMode(cmd.jumpLabel);

        getEncoding(word, am, cmd.jumpLabel, NONE, labelArray, labelArraySize);

        sprintf(temp, "%.4d\t%s\n", *wordCount, word);
        
        strcat(output, temp);

        (*wordCount)++;
    }

    if(cmd.srcAddressing != addressingMode_IRRELEVANT)
        getEncoding(encodingSrc, cmd.srcAddressing, cmd.srcParameter, SOURCE_REGISTER, labelArray, labelArraySize);

    if(cmd.destAddressing != addressingMode_IRRELEVANT)
        getEncoding(encodingDest, cmd.destAddressing, cmd.destParameter, DESTINATION_REGISTER, labelArray, labelArraySize);
    
    if(cmd.srcAddressing == addressingMode_REGISTER_IMMEDIATE && cmd.destAddressing == cmd.srcAddressing) {
        for(i = START_DST ; i >= END_DST ; i--)
            encodingSrc[i] = encodingDest[i];

        sprintf(temp, "%.4d\t%s\n", *wordCount, encodingSrc);
        strcat(output, temp);

        (*wordCount)++;

        return;
    }

    if(cmd.srcAddressing != addressingMode_IRRELEVANT) {
        sprintf(temp, "%.4d\t%s\n", *wordCount, encodingSrc);
        strcat(output, temp);

        (*wordCount)++;
    }

    if(cmd.destAddressing != addressingMode_IRRELEVANT) {
        sprintf(temp, "%.4d\t%s\n", *wordCount, encodingDest);
        strcat(output, temp);

        (*wordCount)++;
    }
}

void getEncoding(char * output, enum addressingModes am, char * param, enum REGISTER_TYPE rt, LabelNode * labelArray, int labelArraySize) {
    int temp;
    char tempString[WORD_LENGTH];
    int i;
    int j;
    LabelNode l;

    memset(tempString, 0, WORD_LENGTH);
    zeroBits(output, WORD_LENGTH);

    switch(am) {
        case addressingMode_REGISTER_IMMEDIATE:
            putARE(output, ENCODING_ABSOLUTE);
            
            temp = getRegisterNumber(param);
            
            getBase2(tempString, temp, BITS_IN_REGISTER_REPRESENTATION);

            if(rt == SOURCE_REGISTER) {
                for(i = START_SRC, j = BITS_IN_REGISTER_REPRESENTATION - 1 ; i >= END_SRC ; i--, j--)
                    output[i] = tempString[j];
            }

            else if(rt == DESTINATION_REGISTER) {
                for(i = START_DST, j = BITS_IN_REGISTER_REPRESENTATION - 1 ; i >= END_DST ; i--, j--)
                    output[i] = tempString[j];
            }

            else {
                for(i = 6, j = 0 ; i <= 11 ; i++, j++)
                    output[i] = tempString[j];
            }

            break;

            case addressingMode_IMMEDIATE:
                putARE(output, ENCODING_ABSOLUTE);
                temp = atoi(param);
                getBase2(tempString, temp, NUM_OF_BITS_WITHOUT_ARE);

                for(i = 0 ; i < NUM_OF_BITS_WITHOUT_ARE ; i++)
                    output[i] = tempString[i];
                
                break;
            
            case addressingMode_DIRECT:
                l = findLabel(labelArray, labelArraySize, param);
                
                if(l.isExternal)
                    putARE(output, ENCODING_EXTERNAL);

                else
                    putARE(output, ENCODING_RELOCATABLE);

                getBase2(tempString, l.labelAddress, NUM_OF_BITS_WITHOUT_ARE);

                for(i = 0 ; i < NUM_OF_BITS_WITHOUT_ARE ; i++)
                    output[i] = tempString[i];

                break;
            
            default: break;
    }
}

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

int getRegisterNumber(char * str) {
    char temp[2];
    temp[0] = str[1];
    temp[1] = '\0';

    return atoi(temp);
}