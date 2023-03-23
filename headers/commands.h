
bool isCommand(char * input);
bool isComment(char * line);
commandOutput parseCommand(char * line);
void handleJumpCommand(commandOutput * output, COMMANDS_INFO cmd, char * line);
void parseImmediate(char * ret, char * word, commandOutput * output);
enum addressingModes getAddressingMode(char * argument);
void getFirstArgument(char *out, char ** line);
void getSecondArgument(char * output, char ** line);
COMMANDS_INFO findCommand(char * word);
bool validAddressingMode(enum addressingModes am, AddressingStruct str);
bool isJumpCommand(enum commandTypes cmdType);
commandOutput createEmptyCommandOutput();
void handleArgument(char * word, commandOutput * output, AddressingStruct legalAddressing, char * number, int argNumber);