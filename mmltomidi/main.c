#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "main.h"
#include "mmlFileStruct.h"

struct mmlFileStruct processedMmlFile; //Necessary global to get information from lex.yy.c
extern FILE *yyin; //For linking to lex.yy.c
extern bool macroEnabled[26]; //Necessary for clearing

int swapIntEndianness(int n) {
	int o = 0;
	
	for (int i = 0; i < 4; i++) {
		*((char *) &o - i + 3) = *((char *) &n + i);
	}
	
	return o;
}

int swapShortEndianness(short n) {
	int o = 0;
	
	for (int i = 0; i < 2; i++) {
		*((char *) &o - i + 1) = *((char *) &n + i);
	}
	
	return o;
}

int writeVariableLengthQuantity(char *ptr, int n) {
	if (!n) {
		*ptr = 0;
		return 1;
	}
	
	int length = 5;
	
	for (int i = 4; i >= 0; i--) {
		if (n >> i * 7) {
			break;
			
		} else {
			length--;
		}
	}
	
	for (int i = length - 1; i >= 0; i--) {
		if (i) {
			*(ptr + length - i - 1) = ((n >> i * 7) & 0x7F) + 0x80;
			
		} else {
			*(ptr + length - i - 1) = (n >> i * 7) & 0x7F;
		}
	}
	
	return length;
}

void writeMTrkHeader(struct mtrkHeader *mtrkHeaderPtr, int trackLength) {
	strncpy(mtrkHeaderPtr->chunkType, "MTrk", 4);
	mtrkHeaderPtr->length = swapIntEndianness(trackLength);
}

void writeMThdHeader(struct mthdHeader *mthdHeaderPtr) {
	strncpy(mthdHeaderPtr->chunkType, "MThd", 4);
	mthdHeaderPtr->length = swapIntEndianness(6);
	mthdHeaderPtr->format = 0;
	mthdHeaderPtr->ntrks = swapShortEndianness(1);
	mthdHeaderPtr->division = swapShortEndianness(8);
}

int generateMIDIFile(char **ptr, struct mmlFileStruct *midiData) {
	//Points ptr towards a malloc assigned array
	
	*ptr = malloc(65536);
	
	if (*ptr == NULL) {
		fprintf(stderr, "Error - memory could not be assigned by malloc\n");
		return NULL;
	}
	
	struct mthdHeader *outputMThdHeader = *ptr;
	struct mtrkHeader *outputMTrkHeader = *ptr + 14;
	char *trackChunkPtr = *ptr + 22;
	
	writeMThdHeader(outputMThdHeader);
	
	if (midiData->name[0]) {
		memcpy(trackChunkPtr, (char []) {0x00, 0xff, 0x03, strlen(midiData->name)}, 4);
		strcpy(trackChunkPtr += 4, midiData->name);
		trackChunkPtr += strlen(midiData->name);
	}
	
	memcpy(trackChunkPtr, (char []) {0x00, 0xFF, 0x58, 0x04, 0x04, 0x02, 0x18, 0x08}, 8); //Time signature
	trackChunkPtr += 8;
	
	memcpy(trackChunkPtr, (char []) {0x00, 0xFF, 0x51, 0x03}, 4); //Tempo
	*((int *) (trackChunkPtr += 4)) = swapIntEndianness(30000000 / 120) >> 8;
	trackChunkPtr += 3;
	
	memcpy(trackChunkPtr, (char []) {0x00, 0xC0, 0x00}, 3); //Default instrument
	trackChunkPtr += 3;
	
	char octave = 4;
	char velocity = 0x7F;
	char transposition = 0;
	char noteLookup[7] = {21, 23, 12, 14, 16, 17, 19};
	char deltaTimeLookup[10] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	for (int i = 0; i < midiData->noteCount; i++) {
		struct note currentNote = midiData->notes[i];
		
		switch (currentNote.command) {
			case 'o':
				octave = currentNote.modifier;
				break;
				
			case '<':
				octave -= octave != 0;
				break;
				
			case '>':
				octave += octave != 9;
				break;
				
			case 'p':
				transposition = currentNote.modifier;
				break;
				
			case 'v':
				velocity = (0x7F * currentNote.modifier) / 9;
				break;
				
			case 't':
				memcpy(trackChunkPtr, (char []) {0x00, 0xFF, 0x51, 0x03}, 4);
				*((int *) (trackChunkPtr += 4)) = swapIntEndianness(30000000 / currentNote.modifier) >> 8;
				trackChunkPtr += 3;
				
				break;
				
			case 'i':
				memcpy(trackChunkPtr, (char []) {0x00, 0xC0, currentNote.modifier}, 3);
				trackChunkPtr += 3;
				
				break;
			
			default:
				;
				char noteNumber = noteLookup[currentNote.command - 'a'] + 12 * octave + currentNote.accidental + transposition;
				
				memcpy(trackChunkPtr, (char []) {0x00, 0x90, (currentNote.command != 'r') * noteNumber, (currentNote.command != 'r') * velocity}, 4);
				trackChunkPtr += 4;
				
				trackChunkPtr += writeVariableLengthQuantity(trackChunkPtr, deltaTimeLookup[currentNote.modifier]);
				memcpy(trackChunkPtr, (char []) {0x80, (currentNote.command != 'r') * noteNumber, (currentNote.command != 'r') * velocity}, 3);
				trackChunkPtr += 3;
				
				break;
		}
	}
	
	memcpy(trackChunkPtr, (char []) {0x00, 0xFF, 0x2F, 0x00}, 4);
	trackChunkPtr += 4;
	
	writeMTrkHeader(outputMTrkHeader, trackChunkPtr - *ptr - 22);

	*ptr = realloc(*ptr, trackChunkPtr - *ptr + 1);

	if (*ptr == NULL) {
		fprintf(stderr, "Error - malloc'd array could not be reallocated\n");
		
		return NULL;
	}
	
	return trackChunkPtr - *ptr;
}

bool fileReadable(char *path) {
	if (access(path, R_OK)) {
		fprintf(stderr, "File %s is not readable\n", path);
		return false;
	}
	
	return true;
}

bool pathValid(char *path) {
	//Currently does not error on a directory

	if (access(path, F_OK)) {
		fprintf(stderr, "File %s does not exist\n", path);
		return false;
	}
	
	return true;
}

bool correctCallForm(int argc, char *argv[]) {
	if ((argc != 2) && (argc != 4)) {
		fprintf(stderr, "Invalid number of arguments given\n");
		
		return false;
	}
	
	return true;
}

int main(int argc, char *argv[]) {
	if (!correctCallForm(argc, argv)) {
		fprintf(stderr, "Usage: mmltomidi [-o output_path] file\n");
		
		return 1;
	}
	
	bool outputPathGiven = !strcmp(argv[1], "-o");
	char *outputPath = (outputPathGiven) ? argv[2] : "output.midi";
	char *inputPath = (outputPathGiven) ? argv[3] : argv[1];

	if ((!pathValid(inputPath)) || (!fileReadable(inputPath))) {
		return 1;
	}
	
	memset(&processedMmlFile, 0, sizeof(processedMmlFile));
	memset(macroEnabled, 0, 26);
	
	yyin = fopen(inputPath, "rb");
	int yyparseResult = yyparse();
	fclose(yyin);
	
	if (yyparseResult == 1) {
		fprintf(stderr, "Syntax error encountered by parser - terminating\n");
		
		return 1;
	}
	
	char *midiBuffer;
	int midiBufferLength = generateMIDIFile(&midiBuffer, &processedMmlFile);
	
	if (midiBuffer == NULL) {
		return 1;
	}
	
	FILE *outputFile = fopen(outputPath, "wb");
	
	if (outputFile == NULL) {
		fprintf(stderr, "Output file could not be created/opened\n");
		
		return 1;
	}
	
	fwrite(midiBuffer, 1, midiBufferLength, outputFile);
	
	free(midiBuffer);
	fclose(outputFile);

	return 0;
}
