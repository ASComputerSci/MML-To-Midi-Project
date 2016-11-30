%{
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"

void yyerror(char *s);

%}

%%
^##.*(\n|(\n\r))	{
	return COMMENT;
	}

^(\n|(\n\r))	{
	return LINE_BREAK;
	}
	
^#tempo" "[0-9]+(\n|(\n\r)) {
	int tempoNumber = atoi(strchr(yytext, ' ') + 1);
	
	if (tempoNumber > 255) {
		yyerror("Syntax error - tempo set above 255"); //Syntax? Should stop program
		
	} else {
		mmlFileStruct.tempo = tempoNumber; //Not initialised in this file - probably bad
	}

	return TEMPO_SET;
	}
	
^#instrument" "[0-9]+(\n|(\n\r)) {
	int instrumentNumber = atoi(strchr(yytext, ' ') + 1);
	
	if (instrumentNumber > 255) {
		yyerror("Syntax error - instrument set above 255"); //Syntax? Should stop program
		
	} else {
		mmlFileStruct.instrument = instrumentNumber; //Not initialised in this file - probably bad
	}

	return INSTRUMENT_SET;
	}
	
^#name" "[a-zA-Z0-9_]+(\n|(\n\r)) {
	mmlFileStruct.name = strchr(yytext, ' ') + 1;

	return NAME_SET;
	}
	
^$[a-z]" "(([cdefgabrov][+-]?[0-9])|($[a-z]))+(\n|(\n\r)) {
	return MACRO_ASSIGNED;
	}
	
^play" "(([cdefgabrov][+-]?[0-9])|($[a-z]))+(\n|(\n\r)) {
	return PLAY_COMMAND;
	}
	
.|\n	{
	return UNRECOGNISED_CHARACTER;
	}
%%
