
bool isCommand(char *);
bool isComment(char *);
commandOutput parseCommand(char *);
void handleJumpCommand(commandOutput *, COMMANDS_INFO, char *);
void parseImmediate(char *, char *, commandOutput *);
enum addressingModes getAddressingMode(char *);
void getFirstArgument(char *, char **);
void getSecondArgument(char *, char **);
COMMANDS_INFO findCommand(char *);
bool validAddressingMode(enum addressingModes, AddressingStruct);
bool isJumpCommand(enum commandTypes);
commandOutput createEmptyCommandOutput();
void handleArgument(char *, commandOutput *, AddressingStruct, char *, int);