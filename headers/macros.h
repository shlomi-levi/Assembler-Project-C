
bool isMacroDefinitionStatement(char *);
void getMacroName(char *, int, char *);
int isExistingMacro(MacroNode *, char *, int);
bool macroEnding(char *);
bool isLegalMacroName(char *);
void addToMacroDefinition(MacroNode *, char *);
MacroNode createEmptyMacro();