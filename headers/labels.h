
LabelNode createEmptyLabel();
LabelNode findLabel(LabelNode * labelArray, int arraySize, char * str);
int findLabelPosition(LabelNode * labelArray, int arraySize, char * str);
void addLabel(LabelNode ** labelArray, int labelArraySize, char * name, bool isExternalLabel, int address);
void addLabelToArray(LabelNode ** labelArray, int labelArraySize, LabelNode ln);
EntryNode createEmptyEntry();
int findEntryPosition(EntryNode * entryArray, int entryArraySize, char * str);
void addEntry(EntryNode ** entryArray, int entryArraySize, char * name, int line);
void addEntryToArray(EntryNode ** entryArray, int entryArraySize, EntryNode e);
void addDataLabel(LabelNode ** labelArray, int labelArraySize, char * name, int * data, int dataLength);
bool isValidLabelName(char * label);
bool labelExistsInArray(LabelNode * labelArray, int labelArraySize, char * str);
bool validateLabels(LabelNode * labelArray, int labelArraySize, EntryNode * entryArray, int entryArraySize, EntryNode * labelsToCheckArray, int labelsToCheckArraySize);
void printProblematicLabels(LabelNode * labelArray, int labelArraySize, EntryNode * entryArray, int entryArraySize, EntryNode * labelsToCheckArray, int labelsToCheckArraySize);
void addAddressesToDataLabels(LabelNode * labelArray, int labelArraySize, int * wordCount);

