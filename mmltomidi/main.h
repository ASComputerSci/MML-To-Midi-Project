#ifndef MAIN_H
#define MAIN_H

extern FILE *yyin; //For linking to lex.yy.c
extern int yyparse (void); //Function prototype for linking to y.tab.c

struct mmlFileStruct {
	char *name;
	unsigned char tempo;
	unsigned char instrument;
}; 

#endif
