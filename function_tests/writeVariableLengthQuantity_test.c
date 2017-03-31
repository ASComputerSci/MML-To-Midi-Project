#include <stdio.h>

struct testCase {
    int input;
    int expectedVLQ;
    int expectedReturn;
};

int writeVariableLengthQuantity(char *outputPtr, int input) {
    if (input == 0) {
        *outputPtr = 0;
        return 1;
    }
    
    int length = 5;
    
    for (int i = 4; i >= 0; i--) {
        if (input >> i * 7) {
            break;
            
        } else {
            length--;
        }
    }
    
    for (int i = length - 1; i >= 0; i--) {
        if (i != 0) {
            *(outputPtr + length - i - 1) = ((input >> i * 7) & 0x7F) + 0x80;
            
        } else {
            *(outputPtr + length - i - 1) = (input >> i * 7) & 0x7F;
        }
    }
    
    return length;
}

int main(int argc, char *argv[]) {
    struct testCase tests[] = {{0x00000000, 0x00000000, 0},
                    {0x00000040, 0x00000040, 1},
                    {0x0000007F, 0x0000007F, 1},
                    {0x00000080, 0x00000081, 2},
                    {0x00002000, 0x000000C0, 2},
                    {0x00003FFF, 0x00007FFF, 2},
                    {0x00004000, 0x00008081, 3},
                    {0x00100000, 0x000080C0, 3},
                    {0x001FFFFF, 0x007FFFFF, 3},
                    {0x00200000, 0x00808081, 4},
                    {0x08000000, 0x008080C0, 4},
                    {0x0FFFFFFF, 0x7FFFFFFF, 4}};
    
    int actualVLQ;
    int actualReturn;
    
    printf("Input    Expected Expected Actual   Actual  \n");
    printf("         VLQ      Return   VLQ      Return  \n");
    
    for (int i = 0; i < sizeof(tests) / sizeof(struct testCase); i++) {
        actualVLQ = 0;
        actualReturn = writeVariableLengthQuantity((char *) &actualVLQ, tests[i].input);
        
        printf("%08X %08X %08X %08X %08X", tests[i].input, tests[i].expectedVLQ, tests[i].expectedReturn, actualVLQ, actualReturn);
        
        if (actualVLQ != tests[i].expectedVLQ) {
            printf(" FAIL\n");
            
            continue;
        }
        
        printf(" PASS\n");
    }
    
    printf("\nNOTE: All VLQ values corrected for endianness\n");
    
    return 0;
}
