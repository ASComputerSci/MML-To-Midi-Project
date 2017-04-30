BUILD_DIR = ./build
STD = -std=c99
OPTI = -O3
EXE = mmltomidi

$(EXE): main.c main.h y.tab.c lex.yy.c
	gcc -g -w $(STD) $(OPTI) -o $(BUILD_DIR)/$(EXE) main.c $(BUILD_DIR)/lex.yy.c $(BUILD_DIR)/y.tab.c

lex.yy.c: lex.l y.tab.h
	lex -s -o $(BUILD_DIR)/lex.yy.c lex.l

y.tab.c y.tab.h: yacc.y
	yacc -d -o $(BUILD_DIR)/y.tab.c yacc.y

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/$(EXE)
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/*.c
	rm -f $(BUILD_DIR)/*.h
