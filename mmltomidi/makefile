BUILD_DIR = ./build
DEBUG = 0
STD = -std=c99
OPT = -Og
EXE = mmltomidi

$(EXE): main.o
	gcc -o $(BUILD_DIR)/$(EXE) $(BUILD_DIR)/main.o

main.o: main.c lex.yy.c y.tab.c
	gcc -D DEBUGGING=$(DEBUG) $(STD) -c -o $(BUILD_DIR)/main.o main.c
	
lex.yy.c: lex.l y.tab.h
ifeq ($(DEBUG), 1)
		lex -s -d -o $(BUILD_DIR)/lex.yy.c lex.l
else
		lex -s -o $(BUILD_DIR)/lex.yy.c lex.l
endif
	
y.tab.c y.tab.h: yacc.y
	yacc -d -o $(BUILD_DIR)/y.tab.c yacc.y

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/$(EXE)
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/*.c
	rm -f $(BUILD_DIR)/*.h

