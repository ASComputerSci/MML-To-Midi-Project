#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "main.h"

bool callValid(int argc, char *argv[]) {
	if ((argc < 2) || (argc > 4)) {
		return false;
	}
	
	if (strcmp(argv[1], "-o") == 0) {
		if (!access(argv[2], F_OK)) {
			fprintf(stderr, "Output file already exists\n");
			
			return false;
		}
		
		if (access(argv[3], F_OK | R_OK)) {
			fprintf(stderr, "Input file cannot be accessed\n");
			
			return false;
		}
		
	} else if (strcmp(argv[2], "-o") == 0) {
		if (access(argv[1], F_OK | R_OK)) {
			fprintf(stderr, "Input file cannot be accessed\n");
			
			return false;
		}
	
		if (!access(argv[3], F_OK)) {
			fprintf(stderr, "Output file already exists\n");
			
			return false;
		}
		
	}
	
	return true;
}

#ifndef UNIT_TESTING

int main(int argc, char *argv[]) {
	if (!callValid(argc, argv)) {
		fprintf(stderr, "Usage: mmltomidi [-o output_path] [file]\n");
		
		return 0;
	}

	return 0;
}

#endif