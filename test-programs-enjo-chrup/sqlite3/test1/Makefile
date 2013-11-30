# Project: test1
# Makefile created by Dev-C++ 4.9.9.2

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
SQLITE3 = ../common/sqlite3
SQLITEMODULE = ../common/sqlitemodule

OBJ  = test1.o $(SQLITEMODULE).o $(SQLITE3).o $(RES)
LINKOBJ  = $(OBJ)
LIBS =  -mwindows -lwsock32 -lpthreadGC2
INCS = 
CXXINCS =
BIN  = test1.exe
CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS)  
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before test1.exe all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $@ $(LIBS)

test1.o: test1.cpp
	$(CPP) -c $< -o $@ $(CXXFLAGS)

$(SQLITEMODULE).o: $(SQLITEMODULE).cpp $(SQLITEMODULE).h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

$(SQLITE3).o: $(SQLITE3).c $(SQLITE3).h
	$(CC) -c $< -o $@ $(CFLAGS)
