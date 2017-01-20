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

/*struct midiEvent {
	char status;
	char *data; // 1 or 2 bytes
};

struct metaEvent {
	char status; //Always FF
	char type;
	char *length; //Variable-length quantity
	char *data;
};

union eventUnion {
	struct midiEvent;
	//struct sysexEvent; Not used in this program
	struct metaEvent;
};

struct mtrkEvent {
	char *deltaTime; //Variable-length quantity
	union eventUnion event;
};*/

struct midiFileTrackChunk {
	char chunkType[4];
	int length;
	//struct mtrkEvent* mtrkEvents;
};

#endif
