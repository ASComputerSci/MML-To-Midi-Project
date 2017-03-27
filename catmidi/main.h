#ifndef MAIN_H
#define MAIN_H

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

struct mtrkEvent {
	char event[259];
	short length;
	int deltaTime;
};

#endif
