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
LIBS=-lcurl

#Paths relative to makefile path
#.h input
IDIR=./include
#.c input
SDIR=./src
#.o output
ODIR=./obj

#Unit Test Path
UTDIR=./unitTest

#Include folder
DFLAGS=-I $(IDIR)

#List your .h and desired .o files here

_DEPS=SimpleDate.hpp StockHistory_Day.hpp StockHistory_Week.hpp HttpDownloader.hpp HttpDownloader_Fidelity.hpp Stock.hpp StockOption.hpp StockRating.hpp OptionStock.hpp OptionStock_Fidelity.hpp StockMarket_NYSE.hpp
_OBJ=SimpleDate.o StockHistory_Day.o StockHistory_Week.o HttpDownloader.o HttpDownloader_Fidelity.o Stock.o StockOption.o StockRating.o OptionStock.o OptionStock_Fidelity.o StockMarket_NYSE.o

#Automatically get the full relevant paths
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

#Executable Source
SMA_COMMON_UT=unitTest/Main_UT.cpp

#Tasks
#Build specific libraries
$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $< $(DFLAGS) $(LIBS)
	@echo Building: $@

#Build unit test
commonUT: $(SMA_COMMON_UT) $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $^ $(DFLAGS) $(LIBS)
	@echo Linking: $@


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ $(SDIR)/*~
	rm -f $(UTDIR)/*.o *~
