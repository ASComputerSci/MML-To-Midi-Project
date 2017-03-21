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
	char event[262]; //Excluding delta time until reassembly
	short length;
	int deltaTime;
};

#endif
