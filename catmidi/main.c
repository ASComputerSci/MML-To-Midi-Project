#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "main.h"

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

int readVariableLengthQuantity(char *ptr) {
	//Currently would segmentation error on a very large variable length quantity
	//Almost certainly can be optimised
	
	char *originalPtr = ptr;
	
	while (*(ptr) & 0x80) {
		ptr++;
	}
	
	int output = 0;
	int outputShift = 0;
	
	do {
		printf("%02X\n", (unsigned char) *ptr);
		
		output |= (*ptr & 0x7F) << outputShift;
		
		outputShift += 7;
		
	} while (ptr-- != originalPtr);
	
	return output;
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
	if (argc < 2) {
		fprintf(stderr, "Too few arguments supplied\n");
		
		return false;
	}
	
	if (argc > 19) {
		fprintf(stderr, "Too many arguments supplied - 16 files can be combined at most\n");
		
		return false;
	}
	
	return true;
}

int main(int argc, char *argv[]) {
	if (!correctCallForm(argc, argv)) {
		fprintf(stderr, "Usage: mmltomidi [-o output_path] [path ...]\n");
		
		return 1;
	}

	bool outputPathGiven = (strcmp(argv[1], "-o")) ? false : true;
	char startOfInputs = (outputPathGiven) ? 3 : 1;
	char numberOfInputs = argc - startOfInputs;

	for (int i = startOfInputs; i < argc; i++) {
		if ((!pathValid(argv[i])) || (!fileReadable(argv[i]))) {
			return 1;
		}
	}
	
	FILE *outputFile = fopen((outputPathGiven) ? argv[2] : "./output.midi", "wb");
	
	if (outputFile == NULL) {
		fprintf(stderr, "Output file could not be created\n");
		
		return 1;
	}
	
	char outputBuffer[16384]; //Note a fixed length of buffer is employed
	
	struct MThd *outputHeader = (void *) outputBuffer;
	struct MTrk *outputTrackHeader = (void *) outputBuffer + 14;
	char *trackPtr = (void *) outputBuffer + 14 + 8;
	
	strncpy(outputHeader->chunkType, "MThd", 4);
	outputHeader->length = bigEndianInt(6);
	outputHeader->format = 0;
	outputHeader->ntrks = bigEndianInt(1);
	outputHeader->division = bigEndianInt(8);
	
	strncpy(outputTrackHeader->chunkType, "MTrk", 4);
	
	FILE *inputFile[numberOfInputs];
	
	for (int i = startOfInputs; i < argc; i++) {
		inputFile[i - startOfInputs] = fopen(argv[i], "rb");
		fseek(inputFile[i - startOfInputs], 14 + 8, SEEK_SET);
	}
	
	
	
	for (int i = 0; i < numberOfInputs; i++) {
		fclose(inputFile[i]);
	}
	
	outputTrackHeader->length = bigEndianInt(trackPtr - outputBuffer - 14 - 8);
	
	fclose(outputFile);

	return 0;
}
