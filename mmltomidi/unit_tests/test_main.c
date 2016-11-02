#include <stdio.h>
#include <stdbool.h>

#define UNIT_TESTING
#include "../main.c"

bool testCallValid() {
	//Assumes the file "a" does not exist
	
	struct testArgs {
		int argc;
		char *argv[5];
		bool expected;

	} testArgList[] = {1, {"mmltomidi"}, false,
					   3, {"mmltomidi", "a", "a"}, false,
					   4, {"mmltomidi", "a", "a", "a"}, false,
					   5, {"mmltomidi", "a", "a", "a", "a"}, false,
					   2, {"mmltomidi", "a"}, false,
					   2, {"mmltomidi", "."}, true,
					   4, {"mmltomidi", "-o", "a", "a"}, false,
					   4, {"mmltomidi", "-o", "a", "."}, true,
					   4, {"mmltomidi", "-o", ".", "a"}, false,
					   4, {"mmltomidi", "-o", ".", "."}, false,
	};
	
	for (int i = 0; i < sizeof(testArgList)/sizeof(struct testArgs); i++) {
		if (callValid(testArgList[i].argc, testArgList[i].argv) != testArgList[i].expected) {
			printf("Call valid failed with arguments: argc = %d, argv = {", testArgList[i].argc);
			
			for (int j = 0; j < testArgList[i].argc - 1; j++) {
				printf("'%s', ", testArgList[i].argv[j]);
			}
			
			printf("'%s'}\n", testArgList[i].argv[testArgList[i].argc - 1]);
			
			return false;
		}
	}

	printf("callValid passed\n");
	return true;
}

int main(int argc, char *argv[]) {
	int tests = 1;
	int testsSuccessful = 0;
	
	testsSuccessful += testCallValid() == true;

	printf("%d/%d tests successful\n", testsSuccessful, tests);
	
	return 0;
}