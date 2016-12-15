#ifndef MMLFILESTRUCT_H
#define MMLFILESTRUCT_H

//Necessary for only one copy of the struct definition

struct mmlFileStruct {
	char name[256]; //Null terminated
	unsigned char tempo;
	unsigned char instrument;
}; 

struct note {
	char command;
	char accidental; //-1 for flat, 1 for sharp
	char modifier;
};

#endif