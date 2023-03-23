#define true 1
#define false 0

#define NOT_IN_MACRO_TABLE -1
#define DOESNT_CONTAIN -1
#define NOT_FOUND -1
#define IRRELEVANT -1
#define ADDRESS_NULL 0

#define NUM_OF_REGISTERS 8

#define MAX_CHARS_PER_COMMAND 100
#define ERR_MESSAGE_LENGTH 200

#define NUM_OF_BITS 14

#define WORD_LENGTH 15
#define BUFFER_SIZE 3000

#define NUM_OF_BITS_WITHOUT_ARE 12

#define BITS_IN_REGISTER_REPRESENTATION 6


/* values inside a data label must be representable with 12 bits. same with immediate values */

#define MIN_VALUE_FOR_LABEL_OR_IMMEDIATE -2048
#define MAX_VALUE_FOR_LABEL_OR_IMMEDIATE 2047

#define MAX_CHARS_IN_IMMEDIATE 4

#define MAX_NUMBER_LENGTH 6

#define MAX_LINE_LENGTH 90

#define MAX_FILENAME_LEN 100

#define MAX_LABEL_LENGTH 31

#define NUM_OF_COMMANDS 16

#define MAX_OUTPUT_WORD 57

#define FIRST_WORD_INDEX_IN_OUTPUT_FILE 100

enum addressingModes {
    addressingMode_INVALID = -2,
    addressingMode_IRRELEVANT,
    addressingMode_IMMEDIATE = 0,
    addressingMode_DIRECT = 1,
    addressingMode_JUMP_ADDRESSING = 2,
    addressingMode_REGISTER_IMMEDIATE = 3
};

enum lineTypes {
    LINE_INVALID,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_COMMAND,
    LINE_DATA,
    LINE_ENTRY,
    LINE_EXTERN
};

enum REGISTER_TYPE {
    NONE,
    SOURCE_REGISTER,
    DESTINATION_REGISTER
};

enum ENCODING {
    ENCODING_ABSOLUTE,
    ENCODING_RELOCATABLE,
    ENCODING_EXTERNAL
};

enum commandTypes {
    MOV = 0,
    CMP = 1,
    ADD = 2,
    SUB = 3,
    NOT = 4,
    CLR = 5,
    LEA = 6,
    INC = 7,
    DEC = 8,
    JMP = 9,
    BNE = 10,
    RED = 11,
    PRN = 12,
    JSR = 13,
    RTS = 14,
    STOP = 15
};