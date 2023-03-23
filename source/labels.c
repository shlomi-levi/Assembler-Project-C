/* This file contains all the functions that are related to labels stuff,
wether they are extern labels, entry labels, or command labels.

Do note that the data structure LabelNode will be used for external labels and command labels,
and the data structure EntryNode will be used for entry labels */

#include "../headers/headers.h"
#include "../headers/labels.h"

/* This function creates an empty label */

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

/* This function finds and returns a label with the name str from the label array. It is already assumed that there is such label. */

LabelNode findLabel(LabelNode * labelArray, int labelArraySize, char * str) {
    int index = findLabelPosition(labelArray, labelArraySize, str); /* Find that label's position in the array, using another function that is in the library */

    return labelArray[index]; /* Return that label */
}

/* This function recieves a label array, the array's size and a string, and returns true wether there is a label with that name in the table,
false otherwise. */

bool labelExistsInArray(LabelNode * labelArray, int labelArraySize, char * str) {
    return (findLabelPosition(labelArray, labelArraySize, str) != NOT_FOUND) ? true : false;
}

/* This function recieves a label array, the array's size and a string, and returns the position in the array in which there's a label
with the name string, or the constant NOT_FOUND if there is no such label. */

int findLabelPosition(LabelNode * labelArray, int arraySize, char * str) {
    int i;

    for(i = 0 ; i < arraySize ; i++) {
        if(!strcmp(labelArray[i].labelName, str))
            return i;
    }

    return NOT_FOUND;
}


/* This function creates a new label and then uses addLabelToArray function to add that label to the labels array */
void addLabel(LabelNode ** labelArray, int labelArraySize, char * name, bool isExternalLabel, int address) {
    LabelNode l = createEmptyLabel(); /* Create an empty label */

    strcpy(l.labelName, name); /* Copy the desired name to that label */

    if(isExternalLabel)
        l.isExternal = true; /* If the label we want to create is external, turn on the external flag*/

    l.labelAddress = address; /* Set the desired label's address. */
    
    addLabelToArray(labelArray, labelArraySize,l); /* Add the label to the array */
}

/* This function adds a label to the labels array, and changes the label array's pointer to point to the new area of memory */

void addLabelToArray(LabelNode ** labelArray, int labelArraySize, LabelNode ln) {
    LabelNode * ret = *labelArray; /* current labels array address */

    /* Allocate space for one more label */
    if(ret == NULL)
        ret = my_malloc(sizeof(LabelNode));
    
    else
        ret = my_realloc(ret, sizeof(LabelNode) * (labelArraySize + 1));

    ret[labelArraySize] = ln; /* add the label to the last place in the labels array */

    *labelArray = ret; /* Make the labels array pointer point to the new area of memory that was allocated */
}


/* This function creates an empty entry */

EntryNode createEmptyEntry() {
    EntryNode e;
    e.labelLine = IRRELEVANT;
    
    return e;
}

/* This function seeks a entry in the entries array (that has the name str) and returns its position,
or the constant NOT_FOUND otherwise. */

int findEntryPosition(EntryNode * entryArray, int entryArraySize, char * str) {
    int i;

    for(i = 0 ; i < entryArraySize ; i++) {
        if(!strcmp(entryArray[i].labelName, str)) /* Loop through the array, and when encountering a entry with the desired name, return its index*/
            return i;
    }

    return NOT_FOUND;
}

/* This function creates a new entry an then uses addEntryToArray to add that entry to the entries array */

void addEntry(EntryNode ** entryArray, int entryArraySize, char * name, int line) {
    EntryNode e = createEmptyEntry();

    strcpy(e.labelName, name);
    e.labelLine = line;

    addEntryToArray(entryArray, entryArraySize, e);
}

/* This function adds a entry to the entries array, It works similarly to addLabelToArray */

void addEntryToArray(EntryNode ** entryArray, int entryArraySize, EntryNode e) {
    EntryNode * ret = *entryArray;

    if(ret == NULL)
        ret = my_malloc(sizeof(EntryNode));
    
    else
        ret = my_realloc(ret, sizeof(EntryNode) * (entryArraySize + 1));

    ret[entryArraySize] = e;

    *entryArray = ret;
}

/* This function creates a new data label and then uses the function addLabelToArray to add that label to the labels array. */

void addDataLabel(LabelNode ** labelArray, int labelArraySize, char * name, int * data, int dataLength) {
    LabelNode l = createEmptyLabel();
    
    strcpy(l.labelName, name); /* Copy the desired labels name */
    l.labelData = data; /* set the label's data */
    l.isDataLabel = true; /* Activate data flag */
    l.dataCount = dataLength; /* Set label's data count */

    addLabelToArray(labelArray, labelArraySize, l);
}

/* This function accepts a string as a parameter and returns true if that parameter is a valid label name,
and false otherwise. */

bool isValidLabelName(char * label) {
    int i;
    int length;
    
    length = strlen(label);

    if(!length) /* Label name cannot be empty */
        return false;

    if(length > MAX_LABEL_LENGTH)
        return false;

    if(!isalpha(label[0])) /* Label name must start with a letter */
        return false;

    for(i = 1 ; i < length ; i++) { /* Label name must contain only alphanumeric characters*/
        if(!isalpha(label[i]) && !isdigit(label[i]))
            return false;
    }
    return true;
}


/* This function finds and returns the problemaic labels (labels that were addressed in the source code, but weren't declared),
or NULL if there are no such labels. */

char * getProblematicLabels(LabelNode * labelArray, int labelArraySize, EntryNode * entryArray, int entryArraySize, EntryNode * labelsToCheckArray, int labelsToCheckArraySize) {
    int i;
    int outSize = 0;
    int tempLen;

    bool found = false;

    char * out = NULL;
    char temp[100];

    memset(temp, 0, 100);

    /* Loop through the entries array to see if there are labels that were declared as entries but weren't declared as data or command labels */
    for(i = 0 ; i < entryArraySize ; i++) {
        if(findLabelPosition(labelArray, labelArraySize, entryArray[i].labelName) != NOT_FOUND)
            continue;
       
        tempLen = sprintf(temp, "Line (%d) - label `%s`\n", entryArray[i].labelLine, entryArray[i].labelName);

        out = (found) ? my_realloc(out, outSize + tempLen + 1) : my_malloc(tempLen + 1);

        outSize += tempLen + 1; /* +1 is for '\0' character */
        
        found = true;

        strcat(out, temp);
    }

    /* The labelsToCheckArray array holds the names of labels that were used as parameters (or as jump labels for jump commands),
    loop through that array and see if it contains labels that weren't declared as data/command/external labels. If there are such labels, print them.*/
    for(i = 0 ; i < labelsToCheckArraySize ; i++) {
        if(findLabelPosition(labelArray, labelArraySize, labelsToCheckArray[i].labelName) != NOT_FOUND)
            continue;

        tempLen = sprintf(temp, "Line (%d) - label `%s`\n", labelsToCheckArray[i].labelLine, labelsToCheckArray[i].labelName);

        out = (found) ? my_realloc(out, outSize + tempLen + 1) : my_malloc(tempLen + 1);

        outSize += tempLen + 1; /* +1 is for '\0' character */
        
        found = true;

        strcat(out, temp);
    }
    return out;
}

/* This function adds addresses to the data labels (we call it at the end of the firstrun
since data labes (variables) get their addresses in the memory after everything else) */

int addAddressesToDataLabels(LabelNode * labelArray, int labelArraySize, int * wordCount) {
    int i;
    int count = 0;

    for(i = 0 ; i < labelArraySize ; i++) {
        LabelNode * l = &labelArray[i];

        if(!l->isDataLabel) /* We are only interested in data labels*/
            continue;
        
        count+= l->dataCount; /* The place that this label "occupies" in the memory is the length of its content. */

        l->labelAddress = (*wordCount); /* Set the label's address */

        (*wordCount) += l->dataCount; /* Increase the wordCount. */
    }

    return count;
}