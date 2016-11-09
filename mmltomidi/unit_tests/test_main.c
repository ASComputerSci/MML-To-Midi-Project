#include <stdio.h>
#include <stdbool.h>

#define UNIT_TESTING
#include "../main.c"

bool testCallValid(int argc, char *argv[], bool expected) {
	char pass = callValid(argc, argv) != expected;

	printf("callValid %s: argc = %d, argv = {", (pass) ? "passed" : "failed", argc);

	for (int i = 0; i < argc - 1; i++) {
		printf("'%s', ", argv[i]);
	}
			
	printf("'%s'}\n", argv[argc - 1]);
		
	return (pass) ? true : false;
}

int main(int argc, char *argv[]) {
	int tests = 1;
	int testsSuccessful = 0;
	
	testsSuccessful += testCallValid(1, (char *[]) {"mmltomidi"}, false);

	printf("%d/%d tests successful\n", testsSuccessful, tests);
	
	return 0;
}

/*testArgList[] = {1, {"mmltomidi"}, false,
					   3, {"mmltomidi", "a", "a"}, false,
					   4, {"mmltomidi", "a", "a", "a"}, false,
					   5, {"mmltomidi", "a", "a", "a", "a"}, false,
					   2, {"mmltomidi", "a"}, false,
					   2, {"mmltomidi", "."}, true,
					   4, {"mmltomidi", "-o", "a", "a"}, false,
					   4, {"mmltomidi", "-o", "a", "."}, true,
					   4, {"mmltomidi", "-o", ".", "a"}, false,
					   4, {"mmltomidi", "-o", ".", "."}, false,
	};*/