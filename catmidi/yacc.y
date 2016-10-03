%{
#include <stdio.h>

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int yywrap() {
	return 0;
}

%}

%token ANYTHING

%%
start_symbol:	ANYTHING
	|	start_symbol ANYTHING
	;
%%
