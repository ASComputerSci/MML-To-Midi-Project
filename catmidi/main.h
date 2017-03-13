#ifndef MAIN_H
#define MAIN_H

struct MThd {
	char chunkType[4];
	int length;
	short format;
	short ntrks;
	short division;
};

struct MTrk {
	char chunkType[4];
	int length;
};

#endif
