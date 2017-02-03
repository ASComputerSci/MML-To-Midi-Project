#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include <math.h>
#include <arpa/inet.h>

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
		
		printf("%02x ", buffer[i]);
	}
	
	printf("\n");
}

int reverseInt(int n) {
	int o = 0;
	
	for (int i = 0; i < sizeof(int); i++) {
		*((char *) &o + sizeof(int) - i - 1) = *((char *) &n + i);
	}
	
	return o;
}

int writeVariableLengthQuantity(char *dest, unsigned int n) {
	//Returns length of variable length quantity written
	
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
			*(dest + i) = (n >> i * 7) & 0x7F;
		
		} else {
			*(dest + i) = ((n >> i * 7) & 0x7F) + 0x80;
		}
	}
	
	return length;
}

void writeMidiFile(FILE *outputFile, struct mmlFileStruct *midiData) {
	struct midiFileHeaderChunk header;
	
	strncpy(header.chunkType, "MThd", 4);
	header.length = 6;
	
	fwrite((void *) &header, sizeof(struct midiFileHeaderChunk), 1, outputFile);
}

bool callValid(int argc, char *argv[]) {
	//Checks calling syntax is correct & files exist
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
	
	if ((inPathIndex != 0) && (access(argv[inPathIndex], F_OK | R_OK))) {
		printError("Input file cannot be accessed");
		
		return false;
	}

	if ((outPathIndex != 0) && (!access(argv[outPathIndex], F_OK))) {
		printError("Output file already exists");
		
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
	
	memset(&processedMmlFile, '\0', sizeof(struct mmlFileStruct)); //Fix this
	
	yyin = fopen(argv[(strcmp(argv[1], "-o")) ? 1 : 3], "rb");
	int yyparseResult = yyparse();
	fclose(yyin);
	
	if (yyparseResult == 1) {
		printError("Syntax error encountered by parser - terminating\n");
		
		return 1;
	}
	
	printDebug("Name set to %s by parser\n", processedMmlFile.name);
	printDebug("Tempo set to %d by parser\n", processedMmlFile.tempo);
	printDebug("Instrument set to %d by parser\n", processedMmlFile.instrument);
	
	FILE *outputFile = fopen("output.midi", "wb"); //Add code to use user set file name
	
	writeMidiFile(outputFile, &processedMmlFile);
	
	fclose(outputFile);

	return 0;
}

#endif
