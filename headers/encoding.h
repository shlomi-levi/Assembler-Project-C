
void getCommandEncoding(char *, commandOutput, LabelNode *, int, EntryNode **, int *, int *);
void getEncoding(char *, enum addressingModes, char *, enum REGISTER_TYPE, LabelNode *, int);
void generateFirstWord(char *, commandOutput);
void putARE(char *, enum ENCODING);
int getRegisterNumber(char *);