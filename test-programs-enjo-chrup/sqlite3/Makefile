# Project: test1
# Makefile created by Dev-C++ 4.9.9.2

PROG1DIR = test1
PROG2DIR = test2-win32-sqlite-testbed
PROG3DIR = test3-tcp-server
PROG4DIR = test4-tcp-client
PROG5DIR = test5-win32-listview
PROG6DIR = test6-wx-listctrl
PROG7DIR = test7-wx-sql-server

PROG1  = test1  
PROG2  = win32-sql-testbed
PROG3  = tcp-server
PROG4  = tcp-client
PROG5  = win32-listview
PROG6  = wx-listctrl
PROG7  = server

INCS = 
CXXINCS =
CXXFLAGS = $(CXXINCS) -Wall
CFLAGS = $(INCS)  
RM = rm -f



.PHONY: all all-before all-after clean clean-deps clean-all 

all: all-before $(PROG1) $(PROG2) $(PROG3) $(PROG4) $(PROG5) $(PROG6) $(PROG7) all-after

$(PROG1): 
	cd $(PROG1DIR) && make

$(PROG2):
	cd $(PROG2DIR) && make

$(PROG3):
	cd $(PROG3DIR) && make
  
$(PROG4):
	cd $(PROG4DIR) && make
  
$(PROG5):
	cd $(PROG5DIR) && make

$(PROG6):
	cd $(PROG6DIR) && make

$(PROG7):
	cd $(PROG7DIR) && make
  
clean-all: clean clean-deps

clean: 
	cd $(PROG1DIR) && make clean
	cd $(PROG2DIR) && make clean
	cd $(PROG3DIR) && make clean
	cd $(PROG4DIR) && make clean
	cd $(PROG5DIR) && make clean
	cd $(PROG6DIR) && make clean
	cd $(PROG7DIR) && make clean
	
clean-deps:
	cd $(PROG1DIR) && make clean-deps
	cd $(PROG2DIR) && make clean-deps
	cd $(PROG3DIR) && make clean-deps
	cd $(PROG4DIR) && make clean-deps
	cd $(PROG5DIR) && make clean-deps
