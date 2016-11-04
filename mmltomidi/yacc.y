%{
#include <stdio.h>

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int yywrap() {
	return 0;
}

%}

%%
start_symbol:	

%%
