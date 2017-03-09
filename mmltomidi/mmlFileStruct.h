#ifndef MMLFILESTRUCT_H
#define MMLFILESTRUCT_H

//Necessary for only one copy of the struct definition

struct note {
	char command; //Letter
	char accidental; //-1 for flat, 1 for sharp
	unsigned char modifier; //Number after
};

struct mmlFileStruct {
	char name[256]; //Null terminated
	
	struct note notes[16384]; //Observe size limit
	int noteCount;
}; 

#endif