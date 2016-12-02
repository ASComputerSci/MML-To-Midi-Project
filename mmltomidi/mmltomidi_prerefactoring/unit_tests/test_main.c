#include <stdio.h>
#include <stdbool.h>

#define UNIT_TESTING
#include "../main.c"

bool callValidWorking() {
	struct callValidTest {
		int argc;
		char *argv[5];
		bool expected;
	
	} callValidTests[] = {1, {"mmltomidi"}, false,
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

	for (int i = 0; i < sizeof(callValidTests)/sizeof(struct callValidTest); i++) {
		bool result = callValid(callValidTests[i].argc, callValidTests[i].argv);
		
		if (result != callValidTests[i].expected) {
			printf("callValid failed with arguments {%d, {", callValidTests[i].argc);
			
			for (int j = 0; j < callValidTests[i].argc - 1; j++) {
				printf("'%s', ", callValidTests[i].argv[j]);
			}
			
			printf("'%s'}} - expected %s got %s\n", callValidTests[i].argv[callValidTests[i].argc - 1], (callValidTests[i].expected) ? "true" : "false", (callValidTests[i].expected) ? "false" : "true");
		
			return false;
		}
	}
	
	printf("callValid passed all tests\n");
	
	return true;
}

int main(int argc, char *argv[]) {
	int tests = 1;
	int testsSuccessful = 0;
	
	testsSuccessful += callValidWorking();

	printf("%d/%d tests successful\n", testsSuccessful, tests);
	
	return 0;
}