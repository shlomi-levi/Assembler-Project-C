#include "../headers/headers.h"
#include "../headers/labels.h"

LabelNode createEmptyLabel() {
    LabelNode l;

    memset(l.labelName, 0, MAX_LABEL_LENGTH);
    
    l.labelData = NULL;
    l.dataCount = 0;
    l.labelAddress = ADDRESS_NULL;
    l.isDataLabel = false;
    l.isExternal = false;

    return l;
}

LabelNode findLabel(LabelNode * labelArray, int labelArraySize, char * str) {
    int index = findLabelPosition(labelArray, labelArraySize, str);

    return labelArray[index];
}

bool labelExistsInArray(LabelNode * labelArray, int labelArraySize, char * str) {
    return (findLabelPosition(labelArray, labelArraySize, str) != NOT_FOUND) ? true : false;
}

int findLabelPosition(LabelNode * labelArray, int arraySize, char * str) {
    int i;

    for(i = 0 ; i < arraySize ; i++) {
        if(!strcmp(labelArray[i].labelName, str))
            return i;
    }

    return NOT_FOUND;
}

void addLabel(LabelNode ** labelArray, int labelArraySize, char * name, bool isExternalLabel, int address) {
    LabelNode l = createEmptyLabel();

    strcpy(l.labelName, name);

    if(isExternalLabel)
        l.isExternal = true;

    l.labelAddress = address;
    
    addLabelToArray(labelArray, labelArraySize,l);
}

void addLabelToArray(LabelNode ** labelArray, int labelArraySize, LabelNode ln) {
    LabelNode * ret = *labelArray;

    if(ret == NULL)
        ret = my_malloc(sizeof(LabelNode));
    
    else
        ret = my_realloc(ret, sizeof(LabelNode) * (labelArraySize + 1));

    ret[labelArraySize] = ln;

    *labelArray = ret;
}

EntryNode createEmptyEntry() {
    EntryNode e;
    e.labelLine = IRRELEVANT;
    
    return e;
}

int findEntryPosition(EntryNode * entryArray, int entryArraySize, char * str) {
    int i;

    for(i = 0 ; i < entryArraySize ; i++) {
        if(!strcmp(entryArray[i].labelName, str))
            return i;
    }

    return NOT_FOUND;
}

void addEntry(EntryNode ** entryArray, int entryArraySize, char * name, int line) {
    EntryNode e = createEmptyEntry();

    strcpy(e.labelName, name);
    e.labelLine = line;

    addEntryToArray(entryArray, entryArraySize, e);
}

void addEntryToArray(EntryNode ** entryArray, int entryArraySize, EntryNode e) {
    EntryNode * ret = *entryArray;

    if(ret == NULL)
        ret = my_malloc(sizeof(EntryNode));
    
    else
        ret = my_realloc(ret, sizeof(EntryNode) * (entryArraySize + 1));

    ret[entryArraySize] = e;

    *entryArray = ret;
}

void addDataLabel(LabelNode ** labelArray, int labelArraySize, char * name, int * data, int dataLength) {
    LabelNode l = createEmptyLabel();
    
    strcpy(l.labelName, name);
    l.labelData = data;
    l.isDataLabel = true;
    l.dataCount = dataLength;

    addLabelToArray(labelArray, labelArraySize, l);
}

bool isValidLabelName(char * label) {
    int i;
    int length;
    
    length = strlen(label);

    if(!length)
        return false;

    if(length > MAX_LABEL_LENGTH)
        return false;

    if(!isalpha(label[0]))
        return false;

    for(i = 1 ; i < length ; i++) {
        if(!isalpha(label[i]) && !isdigit(label[i]))
            return false;
    }
    return true;
}

bool validateLabels(LabelNode * labelArray, int labelArraySize, EntryNode * entryArray, int entryArraySize, EntryNode * labelsToCheckArray, int labelsToCheckArraySize) {
    int i;

    for(i = 0 ; i < entryArraySize ; i++) {
        if(findLabelPosition(labelArray, labelArraySize, entryArray[i].labelName) == NOT_FOUND)
            return false;
    }

    for(i = 0 ; i < labelsToCheckArraySize ; i++) {
        if(findLabelPosition(labelArray, labelArraySize, labelsToCheckArray[i].labelName) == NOT_FOUND)
            return false;
    }

    return true;
}

void printProblematicLabels(LabelNode * labelArray, int labelArraySize, EntryNode * entryArray, int entryArraySize, EntryNode * labelsToCheckArray, int labelsToCheckArraySize) {
    int i;

    for(i = 0 ; i < entryArraySize ; i++) {
        if(findLabelPosition(labelArray, labelArraySize, entryArray[i].labelName) != NOT_FOUND)
            continue;
        
        printf("Line (%d) - label `%s`\n", entryArray[i].labelLine, entryArray[i].labelName);
    }

    for(i = 0 ; i < labelsToCheckArraySize ; i++) {
        if(findLabelPosition(labelArray, labelArraySize, labelsToCheckArray[i].labelName) != NOT_FOUND)
            continue;

        printf("Line (%d) - label `%s`\n", labelsToCheckArray[i].labelLine, labelsToCheckArray[i].labelName);
    }
}

int addAddressesToDataLabels(LabelNode * labelArray, int labelArraySize, int * wordCount) {
    int i;
    int count = 0;

    for(i = 0 ; i < labelArraySize ; i++) {
        LabelNode * l = &labelArray[i];

        if(!l->isDataLabel)
            continue;
        
        count+= l->dataCount;

        l->labelAddress = (*wordCount);

        (*wordCount) += l->dataCount;
    }

    return count;
}