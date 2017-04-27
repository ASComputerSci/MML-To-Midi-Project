#ifndef MMLFILESTRUCT_H
#define MMLFILESTRUCT_H

//Necessary for only one copy of the struct definition to exist

struct note {
	char command;
	char accidental; //-1 for flat, 1 for sharp
	unsigned char modifier;
};

struct mmlFileStruct {
	char name[256]; //Null terminated

	struct note notes[16384];
	int noteCount;
};

#endif
