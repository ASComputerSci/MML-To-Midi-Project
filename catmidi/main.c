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

	for (int i = (strcmp(argv[1], "-o")) ? 1 : 3; i < argc; i++) {
		if ((!pathValid(argv[i])) || (!fileReadable(argv[i]))) {
			return 1;
		}
	}
	
	

	return 0;
}
