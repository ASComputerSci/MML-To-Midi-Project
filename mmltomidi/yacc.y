%{
#include <stdio.h>

#if DEBUGGING
#define YYDEBUG 1
int yydebug = 1;
#endif

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
	|	COMMENT
	;

%%
