# Project: test1
# Makefile created by Dev-C++ 4.9.9.2

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
SQLITE3 = ../common/sqlite3
SQLITEMODULE2 = ../common/sqlitemodule

OBJ  = main.o $(SQLITEMODULE2).o $(SQLITE3).o $(RES)
LINKOBJ  = $(OBJ) 
LIBS =  -mwindows -lwsock32 -lpthreadGC2
INCS = 
CXXINCS =
BIN  = main.exe
CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $@ $(LIBS)

main.o: main.cpp $(SQLITEMODULE2).h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

$(SQLITEMODULE2).o: $(SQLITEMODULE2).cpp $(SQLITEMODULE2).h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

$(SQLITE3).o: $(SQLITE3).c $(SQLITE3).h
	$(CC) -c $< -o $@ $(CFLAGS)
