#ifndef MAIN_H
#define MAIN_H

extern FILE *yyin; //For linking to lex.yy.c
extern int yyparse (void); //Function prototype for linking to y.tab.c

struct midiFileHeaderChunk {
	char chunkType[4];
	int length;
	short format;
	short ntrks;
	short division;
};

struct midiFileTrackChunk {
	char chunkType[4];
	int length;
};

#endif
