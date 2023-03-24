
typedef int bool; /* out of convenience */

/* Macro node data type. every macro has a name, a definition, and size (length of definition)
This data type is being used in the preprocessor phase. */

typedef struct {
    char name[MAX_LINE_LENGTH];
    int size;
    char * definition;
} MacroNode;

/* Legal addressing types data structure. every command has legal addressing types for each of its parameters */

typedef struct {
    bool Addressing_IMMEDIATE;
    bool Addressing_DIRECT;
    bool Addressing_REGISTER_IMMEDIATE;
} AddressingStruct;

/* Commands info data structure, for all the data related to commands (their name, their opcode, etc..)*/

typedef struct {
    char * name;
    enum commandTypes commandType;
    int numOfArguments;
    AddressingStruct legalSrcAddressing;
    AddressingStruct legalDestAddressing;
} COMMANDS_INFO;

/* Label node data structure */
typedef struct {
    char labelName[MAX_LABEL_LENGTH];

    int labelAddress;
    int * labelData;
    int dataCount;

    bool isDataLabel;
    bool isExternal;
    
} LabelNode;

/* Command output data structure. this is the data structure that the parseCommand function returns (in commands.c) */

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

/* Entries node structure
every entry has a name and a line that it was declared in (in the .am file) */

typedef struct {
    char labelName[MAX_LABEL_LENGTH];
    int labelLine;
} EntryNode;


/* Line Classification data type,
Every line has a type, an optional label,
for variable lines (.data or .string lines) there is also data and data length,
For command lines there is the command without label string
If the line is invalid then we have an error message
and the last two booleans are to signify .extern or .entry lines that have labels (so that we would know to print a warning) */

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

