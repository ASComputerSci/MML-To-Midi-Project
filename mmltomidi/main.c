#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include <math.h>

#include "main.h"
#include "mmlFileStruct.h"

#ifndef UNIT_TESTING
#define printError(s) fprintf(stderr, "%s\n", s)
#else
#define printError(s)
#endif

#ifdef DEBUGGING
#define printDebug(...) fprintf(stderr, __VA_ARGS__);
#else
#define printDebug(...);
#endif

struct mmlFileStruct processedMmlFile; //Necessary global to get information from lex.yy.c, gets cleared in main

void printArray(char *buffer, int size) {
	//For debugging, remove in final release
	
	for (int i = 0; i < size; i++) {
		if ((i != 0) && (i % (80 / 3) == 0)) {
			printf("\n");
		}
		
		printf("%02x ", (unsigned char) buffer[i]);
	}
	
	printf("\n");
}

int bigEndianInt(int n) {
	int o = 0;
	
	for (int i = 0; i < sizeof(int); i++) {
		*((char *) &o + sizeof(int) - i - 1) = *((char *) &n + i);
	}
	
	return o;
}

int bigEndianShort(short n) {
	int o = 0;
	
	for (int i = 0; i < sizeof(short); i++) {
		*((char *) &o + sizeof(short) - i - 1) = *((char *) &n + i);
	}
	
	return o;
}

int writeVariableLengthQuantity(char *dest, unsigned int n) {
	//Returns length of variable length quantity written
	//Writes in big endian
	
	if (n == 0) {
		*dest = 0;
		
		return 1;
	}
	
	int length = sizeof(int);
	
	for (int i = sizeof(int) - 1; i >= 0; i--) {
		if (n >> i * 7) {
			break;
			
		} else {
			length--;
		}
	}
	
	for (int i = length - 1; i >= 0; i--) {
		if (i == 0) {
			*(dest + length - i - 1) = (n >> i * 7) & 0x7F;
		
		} else {
			*(dest + length - i - 1) = ((n >> i * 7) & 0x7F) + 0x80;
		}
	}
	
	return length;
}

int generateMIDIFile(char **dest, struct mmlFileStruct *midiData) {
	//Points dest towards a malloc assigned array, null on error, returns length

	*dest = malloc(8192); //Make appropriate estimate!
	
	if (*dest == NULL) {
		fprintf(stderr, "Error - memory could not be assigned by malloc\n");
		return NULL;
	}
	
	struct midiFileHeaderChunk *outputHeader = *dest;
	struct midiFileTrackChunk *outputTrack = *dest + 14;
	
	strncpy(outputHeader->chunkType, "MThd", 4);
	outputHeader->length = bigEndianInt(6);
	outputHeader->format = 0;
	outputHeader->ntrks = bigEndianShort(1);
	outputHeader->division = bigEndianShort(8);
	
	strncpy(outputTrack->chunkType, "MTrk", 4);
	char *trackChunkPtr = *dest + sizeof(struct midiFileHeaderChunk) + sizeof(struct midiFileTrackChunk) - 2;
	
	if (midiData->name[0]) {
		*((int *) trackChunkPtr) = bigEndianInt(0x00FF0300 + strlen(midiData->name)); //Name
		trackChunkPtr += 4;
		strcpy(trackChunkPtr, midiData->name);
		trackChunkPtr += strlen(midiData->name);
	}
	
	*((int *) trackChunkPtr) = bigEndianInt(0x00FF5804); //Time signature
	trackChunkPtr += 4;
	*((int *) trackChunkPtr) = bigEndianInt(0x04021808);
	trackChunkPtr += 4;
	
	*((int *) trackChunkPtr) = bigEndianInt(0x00FF5103); //Default tempo
	trackChunkPtr += 4;
	*((int *) trackChunkPtr) = bigEndianInt(30000000 / 120) >> 8;
	trackChunkPtr += 3;
	
	*((int *) trackChunkPtr) = bigEndianInt(0x00C00000 | (midiData->instrument << 8)); //Instrument
	trackChunkPtr += 3;
	
	char octave = 4;
	char velocity = 0x7F;
	char transposition = 0;
	char defaultLength = 5;
	char noteLookup[7] = {21, 23, 12, 14, 16, 17, 19};
	char deltaTimeLookup[10] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	for (int i = 0; i < midiData->noteCount; i++) {
		struct note currentNote = midiData->notes[i];
		
		switch (currentNote.command) {
			case 'o':
				octave = currentNote.modifier;
				break;
				
			case '<':
				octave -= (octave == 0) ? 0 : 1;
				break;
				
			case '>':
				octave += (octave == 9) ? 0 : 1;
				break;
				
			case 'p':
				transposition = currentNote.modifier;
				break;
				
			case 'v':
				velocity = (0x7F * currentNote.modifier) / 9;
				break;
				
			case 'l':
				defaultLength = currentNote.modifier;
				break;
				
			case 't':
				printf("%d\n", 30000000 / currentNote.modifier);
			
				*((int *) trackChunkPtr) = bigEndianInt(0x00FF5103);
				trackChunkPtr += 4;
				*((int *) trackChunkPtr) = bigEndianInt(30000000 / currentNote.modifier) >> 8;
				trackChunkPtr += 3;
			
			default:
				if (currentNote.modifier == -1) { //Could be done in lex.l? What is best?
					currentNote.modifier = defaultLength;
				}
				
				char noteNumber = noteLookup[currentNote.command - 'a'] + 12 * octave + currentNote.accidental + transposition;
				
				trackChunkPtr += writeVariableLengthQuantity(trackChunkPtr, 4); //Consider delay
				*(trackChunkPtr++) = 0x90;
				*(trackChunkPtr++) = (currentNote.command == 'r') ? 0 : noteNumber;
				*(trackChunkPtr++) = (currentNote.command == 'r') ? 0 : velocity;
				
				trackChunkPtr += writeVariableLengthQuantity(trackChunkPtr, deltaTimeLookup[currentNote.modifier]);
				*(trackChunkPtr++) = 0x80;
				*(trackChunkPtr++) = (currentNote.command == 'r') ? 0 : noteNumber;
				*(trackChunkPtr++) = (currentNote.command == 'r') ? 0 : velocity;
				
				break;
		}
	}
	
	trackChunkPtr += writeVariableLengthQuantity(trackChunkPtr, 0);
	*((int *) trackChunkPtr) = bigEndianInt(0xFF2F0000); //End of Track
	trackChunkPtr += 3;
	
	outputTrack->length = bigEndianInt(trackChunkPtr - *dest - 22);

	*dest = realloc(*dest, trackChunkPtr - *dest + 1);

	if (*dest == NULL) {
		fprintf(stderr, "Error - malloc'd array could not be reallocated\n");
		
		return NULL;
	}
	
	return trackChunkPtr - *dest;
}

bool callValid(int argc, char *argv[]) {
	//Checks calling syntax is correct
	//Currently does not check if the input is a directory

	if ((argc != 2) && (argc != 4)) {
		printError("Invalid number of arguments");
	
		return false;
	}
	
	char inPathIndex = 0;
	char outPathIndex = 0;
	
	if (argc == 2) {
		inPathIndex = 1;
	
	} else if (strcmp(argv[1], "-o") == 0) {
		inPathIndex = 3;
		outPathIndex = 2;
		
	} else {
		printError("-o switch missing");
		
		return false;
	}
	
	if ((inPathIndex != 0) && (access(argv[inPathIndex], F_OK)) && (access(argv[inPathIndex], R_OK))) {
		printError("Input file cannot be accessed");
		
		return false;
	}
	
	if (!access(argv[outPathIndex], F_OK) && access(argv[outPathIndex], W_OK)) {
		printError("Error - output file cannot be written to");
	
		return false;
	}
	
	return true;
}

#ifndef UNIT_TESTING

int main(int argc, char *argv[]) {
	printDebug("Debugging enabled\n");
	
	if (!callValid(argc, argv)) {
		printError("Usage: mmltomidi [-o output_path] [file]");
		
		return 1;
	}
	
	processedMmlFile.name[0] = 0;
	processedMmlFile.instrument = 0;
	processedMmlFile.noteCount = 0;
	
	yyin = fopen(argv[(strcmp(argv[1], "-o")) ? 1 : 3], "rb");
	int yyparseResult = yyparse();
	fclose(yyin);
	
	if (yyparseResult == 1) {
		printError("Syntax error encountered by parser - terminating\n");
		
		return 1;
	}
	
	char *midiBuffer;
	int midiBufferLength = generateMIDIFile(&midiBuffer, &processedMmlFile);
	
	if (midiBuffer == NULL) {
		return 1;
	}
	
	printArray(midiBuffer, midiBufferLength);
	
	FILE *outputFile = fopen("output.midi", "wb"); //Add code to use user set file name
	
	if (outputFile == NULL) {
		fprintf(stderr, "Output file could not be created/opened\n");
		
		return 1;
	}
	
	fwrite(midiBuffer, 1, midiBufferLength, outputFile);
	
	free(midiBuffer);
	fclose(outputFile);

	return 0;
}

#endif
