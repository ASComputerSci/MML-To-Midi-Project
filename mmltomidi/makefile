BUILD_DIR = ./build
DEBUG = 0
STD = -std=c99
OPTI = -O0
EXE = mmltomidi

ifeq ($(DEBUG), 1)
	DEBUGGING = -D DEBUGGING
	LEX_DEBUG = -d
else
	DEBUGGING = 
	LEX_DEBUG =
endif

$(EXE): main.c main.h y.tab.c lex.yy.c
	gcc -g -w $(DEBUGGING) $(STD) $(OPTI) -o $(BUILD_DIR)/$(EXE) main.c $(BUILD_DIR)/lex.yy.c $(BUILD_DIR)/y.tab.c
	
lex.yy.c: lex.l y.tab.h
	lex -s $(LEX_DEBUG) -o $(BUILD_DIR)/lex.yy.c lex.l
	
y.tab.c y.tab.h: yacc.y
	yacc -d -o $(BUILD_DIR)/y.tab.c yacc.y

.PHONY: clean

clean:
	rm -f $(BUILD_DIR)/$(EXE)
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BUILD_DIR)/*.c
	rm -f $(BUILD_DIR)/*.h

