#***************************************************************************************************
#***********************         |\/| o  _ ._ _  |  _   _   _.  _           ************************
#***********************         |  | | (_ | (_) | (/_ (_| (_| (_ \/        ************************
#***********************                                _|        /         ************************
#***********************                      Estd. 2015                    ************************
#***************************************************************************************************
#***************************************************************************************************
#*** The use, disclosure, reproduction, modification, transfer, or transmittal of this work for ****
#*** any purpose in any form or by any means without the written permission of Microlegacy is   ****
#*** strictly prohibited.                                                                       ****
#***                                                                                            ****
#*** Confidential, Unpublished Property of Microlegacy.                                         ****
#*** Use and Distribution Limited Solely to Authorized Personnel.                               ****
#***                                                                                            ****
#*** All Rights Reserved                                                                        ****
#***************************************************************************************************
#Compiler
CC=g++

#Flags
CFLAGS=-pthread -std=c++11 -fpermissive

#Libraries
LIBS=-lcurl -lrt

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
_DEPS=StreamStateMachine.hpp HttpDownloader.hpp
_OBJ=StreamStateMachine.o HttpDownloader.o

#Automatically get the full relevant paths
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

#Executable Source
LL_STATEMACHINE_S=Main.cpp

#Tasks
#Build specific libraries
$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $< $(DFLAGS) $(LIBS)
	@echo Building: $@

#Build main executable
ll_sm:  $(LL_STATEMACHINE_S) $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $^ $(DFLAGS) $(LIBS)
	@echo Linking: $@


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ $(SDIR)/*~ 
