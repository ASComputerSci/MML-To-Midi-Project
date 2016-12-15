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
	int i = 5;
	
	while (yytext[++i] != '\n') {
		processedMmlFile.name[i - 6] = yytext[i];
	}
	
	processedMmlFile.name[i - 6] = '\0';

	return NAME_SET;
	}
	
^$[a-z]" "(([cdefgabrov][+-]?[0-9])|($[a-z]))+(\n|(\r\n)) {
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
	//Replace enabled macros
	//Error on not enabled macros
	//Move processed string of notes into processedMmlFile

	return PLAY_COMMAND;
	}
	
.|\n|\r	{
	return UNRECOGNISED_CHARACTER;
	}
%%
