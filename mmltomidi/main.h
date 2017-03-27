#ifndef MAIN_H
#define MAIN_H

extern int yyparse (void); //Function prototype for linking to y.tab.c

struct mthdHeader {
	char chunkType[4];
	int length;
	short format;
	short ntrks;
	short division;
};

struct mtrkHeader {
	char chunkType[4];
	int length;
};

#endif
