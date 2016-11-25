%{
#include "y.tab.h"

extern void yyerror(char *s);

%}

%%
^##.*(\n|(\n\r))	{
	return COMMENT;
	}

^(\n|(\n\r))	{
	return LINE_BREAK;
	}
	
^#tempo" "[0-9]+(\n|(\n\r)) {
	return TEMPO_SET;
	}
	
^#instrument" "[0-9]+(\n|(\n\r)) {
	return INSTRUMENT_SET;
	}
	
^#name" "[a-zA-Z0-9_]+(\n|(\n\r)) {
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
