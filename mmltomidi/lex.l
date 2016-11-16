%{
#include "y.tab.h"

#define debugPrint(...) if (DEBUGGING) { fprintf(stderr, __VA_ARGS__); }   
%}

%%
^##[.]*[\n(\n\r)]	{
	debugPrint("Comment scanned: %s\n", yytext);
	return COMMENT;
	}

^[\n(\n\r)]	{
	debugPrint("Solitary line break scanned\n");
	return LINE_BREAK;
	}
%%
