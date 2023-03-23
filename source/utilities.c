/* This files contains some utility functions (manily functions that aid with strings operations). */

#include "../headers/headers.h"
#include "../headers/utilities.h"

#define ONE '/'
#define ZERO '.'

#define GetBit(num, bit) (num & (1 << bit)) > 0 ? 1 : 0

/* This function recieves a string and retruns true if this string only contains spaces and tabs, and false otherwise. */
bool onlySpacesAndTabs(char * line) {
    int length = strlen(line);
    int i;
    
    for(i = 0 ; i < length ; i++) {
        if(!isspace(line[i]))
            return false;
    }
    return true;
}

/* This function zero's the cells of a character array (meaning that for each cell it places the character '0') */
void zeroBits(char *src, int count) {
    char zero = '0';

    src[count - 1] = '\0';

    memset(src, zero, count - 1);
}

/* This function recieves a string and a pointer to a string pointer "ptr" as parameters, and places the first word in the string
that "*ptr" points to in the parameter "word". In addition to that it also advances *ptr to get rid of that word
This function returns true if it found a word, and false otherwise */

bool getWord(char * word, int wordLen, char ** ptr) {
    char * text = *ptr;

    int textLen = strlen(text);
    bool foundStart = false;
    int i;

    memset(word, 0, wordLen);

    if(!text[0])
        return false;

    /* Find start */
    i = 0;

    while(i < textLen) {
        if(!isspace(text[i])) {
            *ptr += i; /* We found a start, advance *ptr to point to that start */
            text = *ptr;
            foundStart = true;
            break;
        }
        i++;
    }

    if(!foundStart) /* If we haven't found a start, return false */
        return false;

    /* Find end - meaning find a space or NULL */
    textLen = strlen(text);

    i = 0;

    while(i < textLen) {
        if(isspace(text[i])) {
            memcpy(word, text, i); /* If we found a space, that means we found a word. copy that word to "word" array */
            *ptr += i; /* advance *ptr to get rid of that word */
            return true;
        }

        i++;
    }

    /* If we haven't found an end, then we need the substring from the index 'start', all the way to the end of the string */
    memcpy(word, text, textLen);

    *ptr += textLen; /* Now it will point to '\0' */
    
    return true;
}

/* This function recieves two strings - "str" and "start",
It returns true if the string "str" starts with the characters of the string "start", and false otherwise.*/
bool startsWith(char * str, char * start) {
    int start_len;
    int result;

    start_len = strlen(start);

    if(strlen(str) < start_len) /* If the length of str is shorter than the length of the string "str", then it obviously can't start with the contents of "start" - return false*/
        return false;
    
    if(!start_len) /* Edge case - the string "str" is empty - return true in that case. */
        return true;

    result = strncmp(str, start, start_len); /* Compare the strings "str" and "start". compare only the first "start_len" characters */

    if(!result) /* If they are equal the function strncmp returns 0 */
        return true;

    return false;
}

/* This function recieves two strings - "str" and "end",
It returns true if the string ending of the string "str" is the string "end", and false otherwise. */
bool endsWith(char * str, char * end) {
    int sourceLen = strlen(str);
    int endLen = strlen(end);

    int i;
    int j;

    if(sourceLen < endLen) /* If the length of str is smaller than the length of end, return false*/
        return false;

    if(!endLen) /* Edge case: "end" string is empty, return true in that case. */
        return true;

    /* Compare the last "endLen" characters of "str" with the characters in "end" */
    for(i = sourceLen - endLen, j = 0; i < sourceLen && j < endLen ; i++, j++) {
        if(str[i] != end[j])
            return false; /* If you found a mismatch, return false.*/
    }    

    return true;
}

/* These 3 helper functions for memory allocations, since sometimes memory allocation can fai,
These functions try to allocate memory three times and only after three consecutive failures they crash the program */
void * my_calloc(int num, int size) {
    void * ptr;
    int i;

    for(i = 0 ; i < 3 ; i++) {
        ptr = calloc(num, size);

        if(ptr != NULL)
            return ptr;
    }
    
    printf("Memory allocation failed! Exiting.");
    
    exit(1);

    return NULL;
}

void * my_realloc(void * ptr, int new_size) {
    int i;
    void * new_ptr;

    for(i = 0 ; i < 3 ; i++) {
        new_ptr = realloc(ptr, new_size);

        if(new_ptr != NULL)
            return new_ptr;
    }
    
    printf("Memory allocation failed! Exiting.");
    
    exit(1);
    
    return NULL;
}

void * my_malloc(int size) {
    int i;
    void * ptr;
    
    for(i = 0 ; i < 3 ; i++) {
        ptr = malloc(size);
        if(ptr != NULL)
            return ptr;
    }

    printf("Memory allocation failed! Exiting.");
    
    exit(1);
}

/* This function adds an extension to a file's name. It places the result in the array "out". */
void addExtension(char * out, char * fileName, char * extension) {
    char result[MAX_FILENAME_LEN];
    memset(result, 0, MAX_FILENAME_LEN);


    strcat(result, fileName);
    strcat(result, ".");
    strcat(result, extension);

    strcpy(out, result);
}

/* Returns true if the line is empty, or that the line contains only spaces / tabs */
bool isEmptyLine(char * line) {
    int len = strlen(line);
    int i;

    if(!len)
        return true;

    for(i = 0 ; i < len ; i++) {
        if(!isspace(line[i]))
            return false;
    }

    return true;
}

/* This function gets an integer n and a pointer for a string, and places the binary representation of the
n in the string */
void getBase2(char * string, int n, int numOfBits) {
    bool isPositive = (n >= 0) ? true : false;
    int positive = abs(n);

    int i;
    int j;

    memset(string, 0, numOfBits);

    /* I'm scanning each bit from left to right and concatenating the bit to the string */

    for(i = 0, j = numOfBits - 1 ; j >= 0 ; j--, i++)
        string[i] = (GetBit(positive, j) == 1) ? ONE : ZERO;
    

    string[numOfBits] = '\0'; /* End the string */
    
    if(isPositive)
        return;

    /* If the number is negative, we need to invert the bits and add 1 (this is how
    conversion between a positive number to a negative number is done with 2's complement system ) */

    /* Invert all bits */
    for(i = 0 ; i < numOfBits ; i++)
        string[i] = (string[i] == ZERO) ? ONE : ZERO;

    /* add 1 */
    for(i = numOfBits - 1 ; i >= 0 ; i--) {
        if(string[i] == ZERO) {
            string[i] = ONE;
            break;
        }

        string[i] = ZERO;
    }
}

/* This function removes spaces/tabs/newlines from the ending of a string */
void removeTrailingSpaces(char * string) {
    int i = strlen(string) - 1;

    while(i >= 0) {
        if(!isspace(string[i]))
            break;
        
        string[i] = '\0';
    }
}

/* This function removes spaces/tabs/newlines from the start of a string (until it encounters a character that isn't a space/tab/newline). */
void removeSpacesFromStart(char * string) {
    char tempString[MAX_LINE_LENGTH];

    int len = strlen(string);
    int start = 0;
    int i;

    memset(tempString, 0, MAX_LINE_LENGTH);

    for(i = 0 ; i < len ; i++) {
        if(!isspace(string[i])) {
            start = i;
            break;
        }
    }

    if(!start)
        return;

    memcpy(tempString, string + start, len - start);
    strcpy(string, tempString);
}

/* This function adds a string to a file writing buffer, and if the buffer doesn't have enough space left to concatenate
that string, it flushes it first (meaning - it writes the content of the buffer to the file, and resets the contents of the buffer)
and then adds the string to the buffer. */

void addToBuffer(char * buffer, int bufferSize, char * string, int * bufferLengthPointer, FILE * f) {
    int len = strlen(string);
    int bufferLen = (*bufferLengthPointer); /* Current buffer length */

    if( (bufferSize - bufferLen) < (len + 1)) /* If the buffer doesn't have enough space left to contain the string + a NULL character */
        flushBuffer(buffer, bufferLengthPointer, f); /* Flush the buffer. this function also resets the buffer length */

    strcat(buffer, string); /* add the string to the buffer */
    (*bufferLengthPointer) += len; /* Update the buffer length variable */
}

/* This function flushes a buffer (It writes it's contents in the appropriate file, resets the buffer's data and the buffer's length variable). */
void flushBuffer(char * buffer, int * bufferLengthPointer, FILE * f) {
    int bufferLen = (*bufferLengthPointer); /* Current length of the buffer */
    
    fwrite(buffer, sizeof(char), bufferLen, f); /* Write the buffer's data in the appropriate file. */

    memset(buffer, 0, BUFFER_SIZE); /* Reset the buffer's contents*/
    
    (*bufferLengthPointer) = 0; /* Reset the buffer's length variable. */
}

/* Returns the first occurence of c in str, or "DOESNT_CONTAIN" constant if c is not in str */
int contains(char * str, char c) {
    int len = strlen(str);
    int i;

    for(i = 0 ; i < len ; i++) {
        if(str[i] == c)
            return i;
    }

    return DOESNT_CONTAIN;
}

/* This function places the substring str[start-end] (including the edges) in the parameter dest. */
void substring(char * dest, char * str, int start, int end) {
    int len = end - start + 2;
    int originalLen = strlen(str);

    if(start < 0) /* If the start index is less than 0, then the input is invalid and the function performs nothing. */
        return;

    if(start > end) /* If start > end, then the input is invalid and the function performs nothing. */
        return;

    if(start >= originalLen || end >= originalLen) /* If start/end is greater than the last index, then the input is invalid and the function does nothing. */
        return;
    
    memcpy(dest, str + start, len - 1);
}

/* This function recieves a string and returns true if it represents an integer, false otherwise */
bool isNumber(char * str) {
    int i = 0;
    int len = strlen(str);

    if(!len)
        return false;

    /* If the first character is '-' or '+' then we start the loop from index number 1
    (but first we need to make sure that the string has at least 2 characters) */

    if(str[0] == '-' || str[0] == '+') { 
        i++;

        if(len < 2)
            return false;
    }

    while(i < len) { /* Check to see wether every character is a digit.*/
        if(!isdigit(str[i])) /* If at least one character isn't a digit, return false. */
            return false;
        
        i++;
    }
    return true;
}

/* This function recieves a string and a character c, and returns the number of times the character c appears in the string. */
int countOccurences(char * str, char c) {
    int len = strlen(str);
    int count = 0;
    int i;

    if(!len)
        return 0;

    for(i = 0 ; i < len ; i++) {
        if(str[i] == c)
            count++;
    }
    return count;
}