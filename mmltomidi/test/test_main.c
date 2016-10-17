#include <stdio.h>
#include <stdbool.h>

#define UNIT_TESTING
#include "../main.c"

bool testCallValid() {
	//Assumes the file "a" does not exist

	if (callValid(1, "mmltomidi")) {
		return false;
	}
	
	if (callValid(5, "mmltomidi a b c d")) {
		return false;
	}
	
	if (callValid(2, "mmltomidi a")) {
		return false;
	}
	
	if (!callValid(2, "mmltomidi test_main")) {
		return false;
	}
	
	

	return true;
}

int main(int argc, char *argv[]) {
	int tests = 1;
	int testsSuccessful = 0;
	
	if (testCallValid()) {
		printf("callValid passed");
		testsSuccessful += 1;
		
	else {
		printf("callValid failed");
		
	}

	printf("Performed %d tests, %d successful\n", tests, testsSuccessful);
	
	return 0;
}