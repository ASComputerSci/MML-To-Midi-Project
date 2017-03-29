#include <stdio.h>

struct testCase {
	char input[4];
	int expectedOutput;
};

int readVariableLengthQuantity(char *ptr) {
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

int main(int argc, char *argv[]) {
	struct testCase tests[] = {{{0x00, 0x00, 0x00, 0x00}, 0},
							   {{0x40, 0x00, 0x00, 0x00}, 0x40},
							   {{0x7F, 0x00, 0x00, 0x00}, 0x7F},
							   {{0x81, 0x00, 0x00, 0x00}, 0x80},
							   {{0xC0, 0x00, 0x00, 0x00}, 0x2000},
							   {{0xFF, 0x7F, 0x00, 0x00}, 0x3FFF},
							   {{0x81, 0x80, 0x00, 0x00}, 0x4000},
							   {{0xC0, 0x80, 0x00, 0x00}, 0x100000},
							   {{0xFF, 0xFF, 0x7F, 0x00}, 0x1FFFFF},
							   {{0x81, 0x80, 0x80, 0x00}, 0x200000},
							   {{0xC0, 0x80, 0x80, 0x00}, 0x8000000},
							   {{0xFF, 0xFF, 0xFF, 0x7F}, 0xFFFFFFF}};
	
	int actualOutput;
	
	printf("Input    Expected Actual  \n");
	
	for (int i = 0; i < sizeof(tests) / sizeof(struct testCase); i++) {
		actualOutput = readVariableLengthQuantity(tests[i].input);
		
		for (char c = 0; c < 4; c++) {
			printf("%02X", (unsigned char) tests[i].input[c]);
		}
		
		printf(" %08X %08X", tests[i].expectedOutput, actualOutput);
		
		if (actualOutput != tests[i].expectedOutput) {
			printf(" FAIL\n");
			
			continue;
		}
		
		printf(" PASS\n");
	}
	
	return 0;
}
