
LabelNode createEmptyLabel();
LabelNode findLabel(LabelNode *, int, char *);
int findLabelPosition(LabelNode *, int, char *);
void addLabel(LabelNode **, int, char *, bool, int);
void addLabelToArray(LabelNode **, int, LabelNode);
EntryNode createEmptyEntry();
int findEntryPosition(EntryNode *, int, char *);
void addEntry(EntryNode **, int, char *, int);
void addEntryToArray(EntryNode **, int, EntryNode);
void addDataLabel(LabelNode **, int, char *, int *, int);
bool isValidLabelName(char *);
bool labelExistsInArray(LabelNode *, int, char *);
bool validateLabels(LabelNode *, int, EntryNode *, int, EntryNode *, int);
void printProblematicLabels(LabelNode *, int, EntryNode *, int, EntryNode *, int);
int addAddressesToDataLabels(LabelNode *, int, int *);

