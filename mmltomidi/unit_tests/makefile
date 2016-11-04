BUILD_DIR = ./build
SOURCE_DIR = ..
STD = -std=c99
OPT = -Og
EXE = unit_test

$(EXE): test_main.o
	gcc -o $(BUILD_DIR)/test_main $(BUILD_DIR)/test_main.o
	
test_main.o: test_main.c
	gcc $(STD) -c -o $(BUILD_DIR)/test_main.o test_main.c

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/$(EXE)
	rm -f $(BUILD_DIR)/*.o
