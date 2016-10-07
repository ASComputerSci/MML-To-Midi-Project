#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool callValid(int argc, char *argv[]) {
	if ((argc < 2) || (argc > 4)) {
		return false;
	}
	
	if (strcmp(argv[1], "-o") == 0) {
		//Test in & out file existence and access
		
	} else {
		//Test in & out file existence and access	
		
	}
	
	return true;
}

int main(int argc, char *argv[]) {
	if (!callValid(argc, argv)) {
		fprintf(stderr, "Usage: mmltomidi [-o output_path] [file]\n");
		
		return 0;
	}
	
	

	return 0;
}