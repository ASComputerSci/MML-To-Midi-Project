%{
#include <stdio.h>

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int yywrap() {
	return 1;
}

%}

%start mmlFile
%token COMMENT LINE_BREAK

%%
mmlFile:	line
	|	mmlFile line
	;

line:	LINE_BREAK
	|	COMMENT LINE_BREAK
	;

%%
