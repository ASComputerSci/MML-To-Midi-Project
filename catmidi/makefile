BUILD_DIR = ./build
STD = -std=c99
OPTI = -O0
EXE = catmidi

$(EXE): main.o
	gcc -g -w $(STD) $(OPTI) -o $(BUILD_DIR)/$(EXE) $(BUILD_DIR)/main.o
	
main.o: main.c
	gcc -c $(STD) $(OPTI) -o $(BUILD_DIR)/main.o main.c

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/$(EXE)
	rm -f $(BUILD_DIR)/*.o
