#ifndef MMLFILESTRUCT_H
#define MMLFILESTRUCT_H

//Necessary for only one copy of the struct definition

struct mmlFileStruct {
	char name[256];
	unsigned char tempo;
	unsigned char instrument;
}; 

#endif