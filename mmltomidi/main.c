#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "./build/lex.yy.c"
#include "./build/y.tab.c"

void printError(char *s) {
#ifndef UNIT_TESTING
	fprintf(stderr, "%s\n", s);
#endif
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
	if (!callValid(argc, argv)) {
		printError("Usage: mmltomidi [-o output_path] [file]");
		
		return 1;
	}
	
	FILE *yyin = fopen(argv[(strcmp(argv[1], "-o")) ? 1 : 3], "rb");

	yylex();

	return 0;
}

#endif
