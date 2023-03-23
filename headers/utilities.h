
bool onlySpacesAndTabs(char *);
bool getWord(char *, int, char **);
bool startsWith(char *, char *);
bool endsWith(char *, char *);
void * my_calloc(int, int);
void * my_realloc(void *, int);
void * my_malloc(int);
void addExtension(char *, char *, char *);
bool isEmptyLine(char *);
void getBase2(char *, int, int);
void removeTrailingSpaces(char *);
bool equalsIgnoreSpaces(char *, char *);
int contains(char *, char);
void substring(char *, char *, int, int);
void addToBuffer(char *, int, char *, int *, FILE *);
void flushBuffer(char *, int *, FILE *);
bool isNumber(char *);
void removeSpacesFromStart(char *);
int countOccurences(char *, char);
void convertToUniqueBase2(char *);
void zeroBits(char *, int);