%{
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "y.tab.h"
#include "../mmlFileStruct.h"

extern int yylval;
void yyerror(char *s); //?

extern struct mmlFileStruct processedMmlFile;

char macroTable[26][255]; //Note macro size limit, null terminated
bool macroEnabled[26];

void replaceSubstring(char *source, char *find, char *replace) {
	//Requires an array allocated by a malloc as an input
	
	int matches = 0;
	char *i = strstr(source, find);
	
	while (i != NULL) {
		matches++;
		i = strstr(i + 1, find); //
	}
	
	if (matches == 0) {
		return;
	}
	
	char originalSource[strlen(source)];
	strcpy(originalSource, source); 
	
	source = realloc(source, strlen(source) - matches * strlen(find) + matches * strlen(replace));
	
	if (source == NULL) {
		fprintf(stderr, "Error - memory assigned by malloc could not be reallocated\n");
		source = NULL;
		return;
	}
	
	int oldIndex = 0;
	int newIndex = 0;
	char *nextMatch = NULL;
	
	while (oldIndex != strlen(originalSource)) {
		if (nextMatch == NULL) {
			nextMatch = strstr(originalSource + oldIndex, find);

			if (nextMatch == NULL) {
				strcpy(source + newIndex, originalSource + oldIndex);
				break;
			}
		}
		
		if (nextMatch == originalSource + oldIndex) {
			strcpy(source + newIndex, replace);
			newIndex += strlen(replace);
			oldIndex += strlen(find);
			nextMatch = NULL;
			
		} else {
			source[newIndex++] = originalSource[oldIndex++];
		}
	}
}

%}

%%
^##.*(\n|(\r\n))	{
	return COMMENT;
	}

^(\n|(\r\n))	{
	return LINE_BREAK;
	}
	
^#tempo" "[0-9]+(\n|(\r\n)) {
	int tempoNumber = atoi(yytext + 6);
	
	yylval = tempoNumber;
	
	if (tempoNumber <= 255) { //Aborts in the parser
		processedMmlFile.tempo = tempoNumber;
	}

	return TEMPO_SET;
	}
	
^#instrument" "[0-9]+(\n|(\r\n)) {
	int instrumentNumber = atoi(yytext + 11);
	
	yylval = instrumentNumber;
	
	if (instrumentNumber <= 255) { //Aborts in the parser
		processedMmlFile.instrument = instrumentNumber;
	}

	return INSTRUMENT_SET;
	}
	
^#name" "[a-zA-Z0-9_]+(\n|(\r\n)) {
	//Currently does not error on too long a name
	
	int i = 5;
	
	while (yytext[++i] != '\n') {
		processedMmlFile.name[i - 6] = yytext[i];
	}
	
	processedMmlFile.name[i - 6] = '\0';

	return NAME_SET;
	}
	
^$[a-z]" "(([cdefgabrov][+-]?[0-9])|($[a-z]))+(\n|(\r\n)) {
	//Currently allows multiple definitions of same macro
	//Currently would error on excessive macro length
	
	macroEnabled[yytext[1] - 'a'] = true;
	
	int i = 3;
	
	while (yytext[i] != '\n' && yytext[i] != '\r') {
		macroTable[yytext[1] - 'a'][i - 3] = yytext[i];
		
		i++;
	}
	
	macroTable[yytext[1] - 'a'][i - 3] = '\0';
	
	return MACRO_ASSIGNED;
	}
	
^play" "(([cdefgabrov][+-]?[0-9])|($[a-z]))+(\n|(\r\n)) {
	//Currently ignores unassigned macros being used
	
	char *processedYYText = malloc(strlen(yytext)); //Malloc used for replaceSubstring
	
	if (processedYYText == NULL) {
		fprintf(stderr, "Error - memory could not be allocated by malloc\n");
		return NULL; //Check for correct functioning
	}
	
	strcpy(processedYYText, yytext);
	
	char macroName[] = "$ ";

	for (char i = 0; i < 26; i++) {
		if (macroEnabled[i]) {
			macroName[1] = i + 'a';
			replaceSubstring(processedYYText, macroName, macroTable[i]);
		}
	}
	
	int noteIndex = 0;
	
	for (int i = 5; i < strlen(processedYYText); i++) {
		if (strchr("cdefgabrov", processedYYText[i]) != NULL) {
			processedMmlFile.notes[noteIndex].command = processedYYText[i];
			
		} else if (strchr("0123456789", processedYYText[i]) != NULL) {
			processedMmlFile.notes[noteIndex++].modifier = processedYYText[i] - '0';
			
		} else if (strchr("+-", processedYYText[i]) != NULL) {
			processedMmlFile.notes[noteIndex].accidental = (processedYYText[i] == '+') ? 1 : -1;
			
			if ((i != strlen(processedYYText) - 1) && (strchr("0123456789", processedYYText[i + 1]) == NULL)) {
				noteIndex++;
			}
		}
	}
	
	processedMmlFile.noteCount = noteIndex;
	
	free(processedYYText); // Gives occasional error from here - high priority

	return PLAY_COMMAND;
	}
	
.|\n|\r	{
	return UNRECOGNISED_CHARACTER;
	}
%%
