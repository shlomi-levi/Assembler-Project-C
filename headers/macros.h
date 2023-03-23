
bool isMacroDefinitionStatement(char *);
void getMacroName(char * word, int wordLen, char * line);
int isExistingMacro(MacroNode *, char *, int);
bool macroEnding(char *);
bool isLegalMacroName(char *);
void addToMacroDefinition(MacroNode *, char *);
MacroNode createEmptyMacro();