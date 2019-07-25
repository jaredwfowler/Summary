#Compiler
CC=g++

#Flags
CFLAGS=-pthread -std=c++11 -fpermissive

#Libraries
LIBS=

#Paths relative to makefile path
#.h input
IDIR=./include
#.c input
SDIR=./src
#.o output
ODIR=./obj

#Include folder
DFLAGS=-I $(IDIR)

#List your .h and desired .o files here
#Local
_DEPS=Decrypter424.h WordBank424_English.h
_OBJ=Decrypter424.o WordBank424_English.o

#Automatically get the full relevant paths
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

#Executable Source
EXEC_PROGRAM=main.cpp

#Tasks
#Build specific libraries
$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $< $(DFLAGS) $(LIBS)
	@echo Building: $@

#Build main executable
d424:  $(EXEC_PROGRAM) $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $^ $(DFLAGS) $(LIBS)
	@echo Linking: $@


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ $(SDIR)/*~
