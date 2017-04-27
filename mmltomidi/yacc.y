%{
#include <stdio.h>
#include <stdbool.h>

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int yywrap() {
	return 1;
}

%}

%union {
	int value;
	char errorMessage[256];
}

%start mmlFile

%token <value> COMMENT
%token <value> LINE_BREAK
%token <value> TEMPO_SET
%token <value> INSTRUMENT_SET
%token <value> NAME_SET
%token <value> MACRO_ASSIGNED
%token <value> PLAY_COMMAND
%token <errorMessage> ERROR

%type <value> line

%%
mmlFile:	line
	|	mmlFile line
	;

line:	LINE_BREAK
	|	COMMENT
	|	TEMPO_SET
	|	INSTRUMENT_SET
	|	NAME_SET
	|	MACRO_ASSIGNED
	|	PLAY_COMMAND
	|	ERROR {
		fprintf(stderr, "%s\n", $1);
		YYERROR;
	};
%%
