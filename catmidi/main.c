#include <stdio.h>
#include <stdlib.h>
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
		output |= (*ptr & 0x7F) << outputShift;
		
		outputShift += 7;
		
	} while (ptr-- != originalPtr);

	return output;
}

int writeVariableLengthQuantity(char *dest, unsigned int n) {
	//Returns length of variable length quantity written
	
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

void reassembleMTrkEvent(struct mtrkEvent *event) {
	char buffer[event->length];
	memcpy(buffer, event->event, event->length);
	
	char offset = writeVariableLengthQuantity(event->event, event->deltaTime);
	
	memcpy(event->event + offset, buffer, event->length);
	
	event->length += offset;
}

void readMTrkEvent(unsigned char **input, struct mtrkEvent *outputPtr, char channelNumber) {
	outputPtr->deltaTime = readVariableLengthQuantity((char *) *input);
	
	while (**input & 0x80) {
		(*input)++;
	}
	
	(*input)++;
	
	unsigned char *originalInputPtr = *input;

	switch (**input) {
		case 0xFF:
			(*input)++;
	
			switch (**input) {
				case 0x03: //Name
					(*input)++;
					*input += **input + 1;
				
					break;
			
				case 0x2f: //End
					*input += 2;
			
					break;
			
				case 0x51: //Tempo
					*input += 5;
			
					break;
			
				case 0x58: //Time sig.
					*input += 6;
				
					break;
				
				default:
					//Unknown command, error here
				
					break;
			}
			
			break;
			
		case 0x80: //Note off	
		case 0x90: //Note on
			**input |= channelNumber;
			(*input) += 3;
			
			break;
			
		case 0xC0: //Patch change
			**input |= channelNumber;
			(*input) += 2;
			
			break;
			
		default:
			//Unknown command, error here
		
			break;
	}
	
	outputPtr->length = *input - originalInputPtr;
	memcpy(outputPtr->event, originalInputPtr, outputPtr->length);
}

int swapIntEndian(int n) {
	int o = 0;
	
	for (int i = 0; i < sizeof(int); i++) {
		*((char *) &o + sizeof(int) - i - 1) = *((char *) &n + i);
	}
	
	return o;
}

int swapShortEndian(short n) {
	int o = 0;
	
	for (int i = 0; i < sizeof(short); i++) {
		*((char *) &o + sizeof(short) - i - 1) = *((char *) &n + i);
	}
	
	return o;
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
	
	struct mthd *outputHeader = (void *) outputBuffer;
	struct mtrk *outputTrackHeader = (void *) outputBuffer + 14;
	char *trackPtr = (void *) outputBuffer + 14 + 8;
	
	strncpy(outputHeader->chunkType, "MThd", 4);
	outputHeader->length = swapIntEndian(6);
	outputHeader->format = 0;
	outputHeader->ntrks = swapShortEndian(1);
	outputHeader->division = swapShortEndian(8);
	
	strncpy(outputTrackHeader->chunkType, "MTrk", 4);
	
	unsigned char *inputFileBuffer[numberOfInputs];
	unsigned char *inputFileBufferPtr[numberOfInputs];
	struct mtrkEvent inputEvent[numberOfInputs];
	
	for (int i = 0; i < numberOfInputs; i++) {
		loadFile(argv[i + startOfInputs], &inputFileBuffer[i]);
		inputFileBufferPtr[i] = inputFileBuffer[i] + 14 + 8; //Offset to start of inputs
		
		readMTrkEvent(&inputFileBufferPtr[i], &inputEvent[i], i);
	}
	
	bool nameSet = false;
	bool timeSignatureSet = false;
	
	int lowestDeltaTimeValue;
	char lowestDeltaTimeEventIndex;
	
	char trackNameReference[] = {0xFF, 0x03};
	char timeSignatureReference[] = {0xFF, 0x58, 0x04};
	char endOfTrackReference[] = {0xFF, 0x2F, 0x00};
	
	while (1) {
		lowestDeltaTimeValue = 2147483647;
		lowestDeltaTimeEventIndex = -1;
		
		for (int i = 0; i < numberOfInputs; i++) {
			if (inputEvent[i].deltaTime < lowestDeltaTimeValue) {
				lowestDeltaTimeValue = inputEvent[i].deltaTime;
				lowestDeltaTimeEventIndex = i;
			}
		}
		
		for (int i = 0; i < numberOfInputs; i++) {
			if (i == lowestDeltaTimeEventIndex) {
				continue;
			}
			
			inputEvent[i].deltaTime -= lowestDeltaTimeValue;
		}
		
		if (!memcmp(inputEvent[lowestDeltaTimeEventIndex].event, trackNameReference, 2)) {
			if (nameSet) {
				readMTrkEvent(&inputFileBufferPtr[lowestDeltaTimeEventIndex], &inputEvent[lowestDeltaTimeEventIndex], lowestDeltaTimeEventIndex);
				continue;
			}
			
			nameSet = true;
		}
		
		if (!memcmp(inputEvent[lowestDeltaTimeEventIndex].event, timeSignatureReference, 3)) {
			if (timeSignatureSet) {
				readMTrkEvent(&inputFileBufferPtr[lowestDeltaTimeEventIndex], &inputEvent[lowestDeltaTimeEventIndex], lowestDeltaTimeEventIndex);
				continue;
			}
			
			timeSignatureSet = true;
		}
		
		reassembleMTrkEvent(&inputEvent[lowestDeltaTimeEventIndex]);
		memcpy(trackPtr, inputEvent[lowestDeltaTimeEventIndex].event, inputEvent[lowestDeltaTimeEventIndex].length);
		trackPtr += inputEvent[lowestDeltaTimeEventIndex].length;
		
		if (memcmp(inputEvent[lowestDeltaTimeEventIndex].event + 1, endOfTrackReference, 3) == 0) {
			break;
		}

		readMTrkEvent(&inputFileBufferPtr[lowestDeltaTimeEventIndex], &inputEvent[lowestDeltaTimeEventIndex], lowestDeltaTimeEventIndex);
	}
	
	for (int i = 0; i < numberOfInputs; i++) {
		free(inputFileBuffer[i]);
	}
	
	outputTrackHeader->length = swapIntEndian(trackPtr - outputBuffer - 14 - 8);
	
	fwrite(outputBuffer, 1, trackPtr - outputBuffer, outputFile);
	fflush(outputFile);
	fclose(outputFile);

	return 0;
}
