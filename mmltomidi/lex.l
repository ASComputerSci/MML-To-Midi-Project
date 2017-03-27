%{
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "y.tab.h"
#include "../mmlFileStruct.h"

extern struct mmlFileStruct processedMmlFile;

char macroTable[26][256]; //Note macro size limit, null terminated
bool macroEnabled[26];

void replaceSubstring(char *source, char *find, char *replace) {
	//Requires source to point to a malloc assigned array
	
	int matches = 0;
	char *i = strstr(source, find);
	
	while (i != NULL) {
		matches++;
		i = strstr(i + 1, find);
	}
	
	if (!matches) {
		return;
	}
	
	char originalSource[strlen(source)];
	strcpy(originalSource, source); 
	
	source = realloc(source, strlen(source) - matches * strlen(find) + matches * strlen(replace));
	
	int sourceLen = strlen(source) - matches * strlen(find) + matches * strlen(replace);
	
	if (source == NULL) {
		return;
	}
	
	int oldIndex = 0, newIndex = 0;
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
^##.*(\r|\n|(\r\n))	{
	return COMMENT;
	}

^(\r|\n|(\r\n))	{
	return LINE_BREAK;
	}
	
^#tempo" "[0-9]{1,3}(\r|\n|(\r\n)) {
	int tempoNumber = atoi(yytext + 7);
	
	if (tempoNumber > 255) {
		strcpy(yylval.errorMessage, "Error - tempo set higher than 255");
		return ERROR;
	}

	processedMmlFile.notes[processedMmlFile.noteCount].command = 't';
	processedMmlFile.notes[processedMmlFile.noteCount++].modifier = tempoNumber;

	return TEMPO_SET;
	}
	
^#instrument" "[0-9]+(\r|\n|(\r\n)) {
	int instrumentNumber = atoi(yytext + 11);
	
	if (instrumentNumber > 127) {
		strcpy(yylval.errorMessage, "Error - instrument set higher than 127");
		return ERROR;
	}
	
	processedMmlFile.notes[processedMmlFile.noteCount].command = 'i';
	processedMmlFile.notes[processedMmlFile.noteCount++].modifier = instrumentNumber;

	return INSTRUMENT_SET;
	}
	
^#name" "[a-zA-Z0-9_]+(\r|\n|(\r\n)) {
	int i = 5;
	
	while (yytext[i] != '\n' && yytext[i] != '\r') {
		if (i == 260) {
			strcpy(yylval.errorMessage, "Error - name set was too long: limit is 255");
			return ERROR;
		}
		
		processedMmlFile.name[i - 6] = yytext[i++];
	}
	
	processedMmlFile.name[i - 6] = '\0';

	return NAME_SET;
	}
	
^$[a-z]" "(([cdefgabrov][+-]?[0-9]?)|($[a-z])|[<>]|(p[0-9]+)|(l[0-9])|" ")+(\r|\n|(\r\n)) {
	int i = 3;
	
	while (yytext[i] != '\n' && yytext[i] != '\r') {
		if (i == 258) {
			strcpy(yylval.errorMessage, "Macro assigned was too long - limit is 255");
			return ERROR;
		}
	
		macroTable[yytext[1] - 'a'][i - 3] = yytext[i++];
	}
	
	macroEnabled[yytext[1] - 'a'] = true;
	macroTable[yytext[1] - 'a'][i - 3] = '\0';
	
	return MACRO_ASSIGNED;
	}
	
^play" "(([cdefgabrov][+-]?[0-9]?)|($[a-z])|[<>]|(p[0-9]{1,2})|(l[0-9])|" ")+(\r|\n|(\r\n)) {
	char *processedYYText = malloc(strlen(yytext)); //Malloc needed for replaceSubstring
	
	if (processedYYText == NULL) {
		strcpy(yylval.errorMessage, "Error - memory could not be malloc'd");
		return ERROR;
	}
	
	strcpy(processedYYText, yytext);
	
	for (int i = 0; i < strlen(yytext); i++) {
		if ((yytext[i] == '$') && (!macroEnabled[yytext[i + 1] - 'a'])) {
			strcpy(yylval.errorMessage, "Macro used but not assigned");
			return ERROR;
		}
	}
	
	for (char i = 0; i < 26; i++) {
		if (macroEnabled[i]) {
			replaceSubstring(processedYYText, (char []) {'$', i + 'a', 0x00}, macroTable[i]);
			
			if (processedYYText == NULL) {
				strcpy(yylval.errorMessage, "Error - memory assigned by malloc could not be reallocated");
				return ERROR;
			}
		}
	}
	
	int index = 5;
	char defaultLength = 5;
	
	while (index < strlen(processedYYText)) {
		if (strchr("cdefgabrov", processedYYText[index]) != NULL) {
			processedMmlFile.notes[processedMmlFile.noteCount].command = processedYYText[index++];
			processedMmlFile.notes[processedMmlFile.noteCount].accidental = 0;
			processedMmlFile.notes[processedMmlFile.noteCount].modifier = defaultLength;
			
			while (1) {
				if ((index < strlen(processedYYText)) && (strchr("+-", processedYYText[index]) != NULL)) {
					processedMmlFile.notes[processedMmlFile.noteCount].accidental = (processedYYText[index++] == '+') ? 1 : -1;
					
					continue;
				}
				
				if ((index < strlen(processedYYText)) && (strchr("0123456789", processedYYText[index]) != NULL)) {
					processedMmlFile.notes[processedMmlFile.noteCount].modifier = processedYYText[index++] - '0';
					
					continue;
				}
				
				break;
			}
			
			processedMmlFile.noteCount++;
			
		} else if (strchr("<>", processedYYText[index]) != NULL) {
			processedMmlFile.notes[processedMmlFile.noteCount++].command = processedYYText[index++];
			
		} else if (processedYYText[index] == 'p') {
			processedMmlFile.notes[processedMmlFile.noteCount].command = processedYYText[index++];
			processedMmlFile.notes[processedMmlFile.noteCount].modifier = 0;
			
			if ((index + 1 < strlen(processedYYText)) && (strchr("0123456789", processedYYText[index + 1]) != NULL)) {
				processedMmlFile.notes[processedMmlFile.noteCount].modifier = (processedYYText[index++] - '0') * 10;
			}
			
			processedMmlFile.notes[processedMmlFile.noteCount].modifier += processedYYText[index++] - '0';
			
			if (processedMmlFile.notes[processedMmlFile.noteCount++].modifier > 11) {
				strcpy(yylval.errorMessage, "Transposition set too high - only values from 0 to 11 are valid");
				return ERROR;
			}
			
		} else if (processedYYText[index] == 'l') {
			defaultLength = processedYYText[++index] - '0';
			index++;
		
		} else {
			index++;
		}
	}

	free(processedYYText);

	return PLAY_COMMAND;
}
	
.|\n|\r	{
	return ERROR;
	}
%%
