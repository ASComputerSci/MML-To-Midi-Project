%{
#include <stdio.h>
#include <stdbool.h>

#if DEBUGGING
#define YYDEBUG 1
int yydebug = 1;
#endif

bool nameSet = false;
bool tempoSet = false;
bool instrumentSet = false;

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int yywrap() {
	return 1;
}

%}

%start mmlFile
%token COMMENT LINE_BREAK TEMPO_SET INSTRUMENT_SET NAME_SET MACRO_ASSIGNED PLAY_COMMAND UNRECOGNISED_CHARACTER

%%
mmlFile:	line
	|	mmlFile line
	;

line:	LINE_BREAK
	|	COMMENT
	|	TEMPO_SET {
		if ($$ > 255) {
			yyerror("Error - tempo set higher than 255");
			YYERROR;
			
		} else if (tempoSet) {
			yyerror("Error - tempo set more than once");
			YYERROR;
		
		} else {
			tempoSet = true;
		}
	}	
	|	INSTRUMENT_SET	{
		if ($$ > 255) {
			yyerror("Error - instrument set higher than 255");
			YYERROR;
			
		} else if (instrumentSet) {
			yyerror("Error - instrument set more than once");
			YYERROR;
		
		} else {
			instrumentSet = true;
		}
	}
	|	NAME_SET {
		if (nameSet) {
			yyerror("Error - name set more than once");
			YYERROR;
		
		} else {
			nameSet = true;
		}
	}
	|	MACRO_ASSIGNED
	|	PLAY_COMMAND
	;
%%
