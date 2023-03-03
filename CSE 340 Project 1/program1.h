#ifndef PROGRAM1_H
#define PROGRAM1_H

/*** Below is the list of functions that must be implemented by the program. ***/

// Loads the grammar.
void LoadGrammar(char *grammarFileName);

// Returns a string with some statistics for the grammar.
char* GetStats();

// Returns the first sets of all nonterminals.
char* GetFirstSets();

// Returns the first set of the specified nonterminal.
char* GetFirstSet(char *nonTerminal);

// Returns the follow sets of all nonterminals.
char* GetFollowSets();

// Returns the follow set of the specified nonterminal.
char* GetFollowSet(char *nonTerminal);

#endif