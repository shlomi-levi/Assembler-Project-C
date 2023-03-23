
void getCommandEncoding(char * output, commandOutput cmd, LabelNode * labelArray, int labelArraySize, int * wordCount);
void getEncoding(char * output, enum addressingModes am, char * param, enum REGISTER_TYPE rt, LabelNode * labelArray, int labelArraySize);
void generateFirstWord(char * output, commandOutput cmd);
void putARE(char * output, enum ENCODING e);
int getRegisterNumber(char * str);