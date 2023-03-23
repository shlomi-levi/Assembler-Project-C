
typedef int bool;

typedef struct {
    char name[MAX_LINE_LENGTH];
    int size;
    char * definition;
} MacroNode;

typedef struct {
    bool Addressing_IMMEDIATE;
    bool Addressing_DIRECT;
    bool Addressing_REGISTER_IMMEDIATE;
} AddressingStruct;

typedef struct {
    char * name;
    enum commandTypes commandType;
    int numOfArguments;
    AddressingStruct legalSrcAddressing;
    AddressingStruct legalDestAddressing;
} COMMANDS_INFO;

typedef struct {
    char labelName[MAX_LABEL_LENGTH];

    int labelAddress;
    int * labelData;
    int dataCount;

    bool isDataLabel;
    bool isExternal;
    
} LabelNode;

typedef struct {
    bool isJumpAddressing;
    char jumpLabel[MAX_LABEL_LENGTH];
    enum addressingModes srcAddressing;
    enum addressingModes destAddressing;
    enum commandTypes commandType;
    char srcParameter[MAX_LINE_LENGTH];
    char destParameter[MAX_LINE_LENGTH];
    int numOfWords;
    bool validCommand;
    char errorMessage[ERR_MESSAGE_LENGTH];
} commandOutput;

typedef struct {
    char labelName[MAX_LABEL_LENGTH];
    int labelLine;
} EntryNode;

typedef struct {
    enum lineTypes type;
    char label[MAX_LABEL_LENGTH];
    int * data;
    int dataLength;
    char commandWithoutLabel[MAX_LINE_LENGTH];
    char errorMessage[ERR_MESSAGE_LENGTH];
    bool labelForExtern;
    bool labelForEntry;
} LineClassification;

