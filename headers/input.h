
LineClassification createEmptyLineClassification();
LineClassification classifyLine(char * line, bool isSecondRun);
void readData(LineClassification * lc, char * line);
void readString(LineClassification * lc, char * line);
void handleExternOrEntry(LineClassification * lc, char * line);