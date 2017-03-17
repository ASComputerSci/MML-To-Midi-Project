#ifndef MAIN_H
#define MAIN_H

struct mthd {
	char chunkType[4];
	int length;
	short format;
	short ntrks;
	short division;
};

struct mtrk {
	char chunkType[4];
	int length;
};

struct mtrkEvent {
	char event[259]; //Including delta time
	short length;
	int deltaTime;
};

#endif
