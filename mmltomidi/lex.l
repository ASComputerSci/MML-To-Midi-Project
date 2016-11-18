%{
#include "y.tab.h"

%}

%%
^##.*[\n(\n\r)]?	{
	return COMMENT;
	}

^[\n(\n\r)]	{
	return LINE_BREAK;
	}
	

%%
