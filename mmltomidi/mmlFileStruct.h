#ifndef MMLFILESTRUCT_H
#define MMLFILESTRUCT_H

//Necessary for only one copy of the struct definition

struct note {
	char command; //Letter
	char accidental; //-1 for flat, 1 for sharp
	char modifier; //Number after
};

struct mmlFileStruct {
	char name[256]; //Null terminated
	unsigned char tempo;
	unsigned char instrument;
	
	struct note *notes;
}; 

#endif