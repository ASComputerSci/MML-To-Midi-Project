%{
#include <stdio.h>

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int yywrap() {
	return 1;
}

%}

%token COMMENT LINE_BREAK

%%
mmlFile:	line { debugPrint("Reduced to start symbol\n") }
	|	mmlFile line
	;

line:	LINE_BREAK
	|	COMMENT LINE_BREAK
	;

%%
