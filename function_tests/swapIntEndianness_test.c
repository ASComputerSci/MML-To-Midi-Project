#include <stdio.h>

struct testCase {
	int input;
	int expectedOutput;
};

int swapIntEndianness(int n) {
	int o = 0;
	
	for (int i = 0; i < 4; i++) {
		*((char *) &o - i + 3) = *((char *) &n + i);
	}
	
	return o;
}

int main(int argc, char *argv[]) {
	struct testCase tests[] = {{0x00000000, 0x00000000},
							   {0x000000FF, 0xFF000000},
							   {0x0000FF00, 0x00FF0000},
							   {0x00FF0000, 0x0000FF00},
							   {0xFF000000, 0x000000FF},
							   {0x12345678, 0x78563412}};
	
	int actualOutput;
	
	printf("Input    Expected Actual  \n");
	
	for (int i = 0; i < sizeof(tests) / sizeof(struct testCase); i++) {
		actualOutput = swapIntEndianness(tests[i].input);
		
		printf("%08X %08X %08X", tests[i].input, tests[i].expectedOutput, actualOutput);
		
		if (actualOutput != tests[i].expectedOutput) {
			printf(" FAIL\n");
			
			continue;
		}
		
		printf(" PASS\n");
	}
	
	return 0;
}
