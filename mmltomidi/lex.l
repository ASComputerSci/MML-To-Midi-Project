%{
#include "y.tab.h"

#define debugPrint(s) if (DEBUGGING) { fprintf(stderr, s); }   

%}

%%
##[. ]*	{
	debugPrint("Comment scanned\n");
	return COMMENT;
	}

[\n\r]+	{
	debugPrint("Line break\n");
	return LINE_BREAK;
	}
%%
