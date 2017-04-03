#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "main.h"

int readVariableLengthQuantity(char *inputPtr) {
	char *workingPtr = inputPtr;
	
	while (*workingPtr & 0x80) {
		workingPtr++;
	}
	
	int output = 0;
	int outputShift = 0;

	do {
		output |= (*workingPtr & 0x7F) << outputShift;
		
		outputShift += 7;
		
	} while (workingPtr-- != inputPtr);

	return output;
}

int writeVariableLengthQuantity(char *outputPtr, int input) {
	if (input == 0) {
		*outputPtr = 0;
		return 1;
	}
	
	int length = 5;
	
	for (int i = 4; i >= 0; i--) {
		if (input >> i * 7) {
			break;
			
		} else {
			length--;
		}
	}
	
	for (int i = length - 1; i >= 0; i--) {
		if (i != 0) {
			*(outputPtr + length - i - 1) = ((input >> i * 7) & 0x7F) + 0x80;
			
		} else {
			*(outputPtr + length - i - 1) = (input >> i * 7) & 0x7F;
		}
	}
	
	return length;
}

char readMTrkEvent(unsigned char **inputPP, struct mtrkEvent *outputPtr, char channelNumber) {
	//Returns non-zero on error
	
	outputPtr->deltaTime = readVariableLengthQuantity((char *) *inputPP);

	while (**inputPP & 0x80) {
		(*inputPP)++;
	}
	
	(*inputPP)++;
	
	unsigned char *originalPtr = *inputPP;

	switch (**inputPP) {
		case 0xFF:
			(*inputPP)++;
	
			switch (**inputPP) {
				case 0x03: //Name
					(*inputPP)++;
					*inputPP += **inputPP + 1;
				
					break;
			
				case 0x2f: //End
					*inputPP += 2;
			
					break;
			
				case 0x51: //Tempo
					*inputPP += 5;
			
					break;
			
				case 0x58: //Time sig.
					*inputPP += 6;
				
					break;
				
				default:
					fprintf(stderr, "Unknown MTrk event encountered\n");
				
					return 1;
			}
			
			break;
			
		case 0x80: //Note off	
		case 0x90: //Note on
			**inputPP |= channelNumber;
			*inputPP += 3;
			
			break;
			
		case 0xC0: //Patch change
			**inputPP |= channelNumber;
			*inputPP += 2;
			
			break;
			
		default:
			fprintf(stderr, "Unknown MTrk event encountered\n");
		
			return 1;
	}

	outputPtr->length = *inputPP - originalPtr;
	memcpy(outputPtr->event, originalPtr, outputPtr->length);
	
	return 0;
}

int swapIntEndianness(int input) {
	int output = 0;
	
	for (int i = 0; i < sizeof(int); i++) {
		*((char *) &output + sizeof(int) - i - 1) = *((char *) &input + i);
	}
	
	return output;
}

int swapShortEndianness(short input) {
	int output = 0;
	
	for (int i = 0; i < sizeof(short); i++) {
		*((char *) &output + sizeof(short) - i - 1) = *((char *) &input + i);
	}
	
	return output;
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

int combineMIDIFiles(char *outputBuffer, unsigned char *inputBuffer[], char inputBufferCount) {
	//Returns zero on error
	
	struct mthdHeader *outputMThdHeader = (void *) outputBuffer;
	struct mtrkHeader *outputMTrkHeader = (void *) outputBuffer + 14;
	char *trackPtr = (void *) outputBuffer + 22;
	
	writeMThdHeader(outputMThdHeader);
	
	unsigned char *inputBufferPtr[inputBufferCount];
	struct mtrkEvent inputEvent[inputBufferCount];
	
	for (int i = 0; i < inputBufferCount; i++) {
		inputBufferPtr[i] = inputBuffer[i] + 22;
		
		if (readMTrkEvent(&inputBufferPtr[i], &inputEvent[i], i)) {
			return 0;
		}
	}
	
	bool nameSet = false;
	bool timeSignatureSet = false;
	
	int smallestDeltaTime;
	char soonestEventIndex;
	struct mtrkEvent *soonestEvent;
	
	while (1) {
		smallestDeltaTime = inputEvent[0].deltaTime;
		soonestEventIndex = 0;
		
		for (int i = 1; i < inputBufferCount; i++) {
			if (inputEvent[i].deltaTime < smallestDeltaTime) {
				smallestDeltaTime = inputEvent[i].deltaTime;
				soonestEventIndex = i;
			}
		}
		
		for (int i = 0; i < inputBufferCount; i++) {
			if (i == soonestEventIndex) {
				continue;
			}
			
			inputEvent[i].deltaTime -= smallestDeltaTime;
		}
		
		soonestEvent = &inputEvent[soonestEventIndex];
		
		if (!memcmp(soonestEvent->event, (char []) {0xFF, 0x03}, 2)) { //Name setting event
			if (nameSet) {
				if (readMTrkEvent(&inputBufferPtr[soonestEventIndex], &inputEvent[soonestEventIndex], soonestEventIndex)) {
					return 0;
				}
				
				continue;
			}

			nameSet = true;
		}
		
		if (!memcmp(soonestEvent->event, (char []) {0xFF, 0x58, 0x04}, 3)) { //Time signature setting event
			if (timeSignatureSet) {
				if (readMTrkEvent(&inputBufferPtr[soonestEventIndex], &inputEvent[soonestEventIndex], soonestEventIndex)) {
					return 0;
				}
				
				continue;
			}
			
			timeSignatureSet = true;
		}
		
		trackPtr += writeVariableLengthQuantity(trackPtr, soonestEvent->deltaTime);
		memcpy(trackPtr, soonestEvent->event, soonestEvent->length);
		trackPtr += soonestEvent->length;
		
		if (memcmp(soonestEvent->event, (char []) {0xFF, 0x2F, 0x00}, 3) == 0) { //End of track event	
			break;
		}
		
		if (readMTrkEvent(&inputBufferPtr[soonestEventIndex], &inputEvent[soonestEventIndex], soonestEventIndex)) {
			return 0;
		}
	}
	
	writeMTrkHeader(outputMTrkHeader, trackPtr - outputBuffer - 22);
	
	return trackPtr - outputBuffer;
}

int loadFile(char *path, unsigned char **buffer) {
	FILE *f = fopen(path, "rb");
	
	fseek(f, 0, SEEK_END);
	int length = ftell(f);
	rewind(f);
	
	*buffer = malloc(length);
	fread(*buffer, 1, length, f);
	
	fclose(f);
	
	return length;
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
		fprintf(stderr, "Usage: catmidi [-o output_path] [path ...]\n");
		
		return 1;
	}

	bool outputPathGiven = !strcmp(argv[1], "-o");
	char startOfInputs = (outputPathGiven) ? 3 : 1;
	char numberOfInputs = argc - startOfInputs;

	for (int i = startOfInputs; i < argc; i++) {
		if ((!pathValid(argv[i])) || (!fileReadable(argv[i]))) {
			return 1;
		}
	}
	
	char outputBuffer[65536];
	unsigned char *inputBuffer[numberOfInputs];
	
	for (int i = 0; i < numberOfInputs; i++) {
		loadFile(argv[i + startOfInputs], &inputBuffer[i]);
	}
	
	int outputBufferLength = combineMIDIFiles(outputBuffer, inputBuffer, numberOfInputs);
	
	for (int i = 0; i < numberOfInputs; i++) {
		free(inputBuffer[i]);
	}
	
	if (outputBufferLength == 0) {
		return 1;
	}
	
	FILE *outputFile = fopen((outputPathGiven) ? argv[2] : "./output.midi", "wb");
	
	if (outputFile == NULL) {
		fprintf(stderr, "Output file could not be created\n");
		
		return 1;
	}
	
	fwrite(outputBuffer, 1, outputBufferLength, outputFile);
	fflush(outputFile);
	fclose(outputFile);

	return 0;
}
