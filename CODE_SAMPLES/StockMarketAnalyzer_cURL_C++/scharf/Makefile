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

#Paths of common files used in multiple projects
#.h input
IDIR_C=../common/include
#.c input
SDIR_C=../common/src
#.o output
ODIR_C=../common/obj

#Unit Test Path
UTDIR=./unitTest

#Include folder
DFLAGS=-I $(IDIR) -I $(IDIR_C)

#List your .h and desired .o files here
#Common
_DEPS_C=SimpleDate.hpp StockHistory_Day.hpp StockHistory_Week.hpp HttpDownloader.hpp HttpDownloader_Fidelity.hpp Stock.hpp StockOption.hpp StockRating.hpp OptionStock.hpp OptionStock_MarketWatch.hpp StockMarket_NYSE.hpp
_OBJ_C=SimpleDate.o StockHistory_Day.o StockHistory_Week.o HttpDownloader.o HttpDownloader_Fidelity.o Stock.o StockOption.o StockRating.o OptionStock.o OptionStock_Fidelity.o OptionStock_MarketWatch.o StockMarket_NYSE.o
#Local
_DEPS=ScharfOptionStock.hpp ScharfMethod.hpp
_OBJ=ScharfOptionStock.o ScharfMethod.o

#Automatically get the full relevant paths
DEPSC=$(patsubst %,$(IDIR_C)/%,$(_DEPS_C))
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
OBJC=$(patsubst %,$(ODIR_C)/%,$(_OBJ_C))

#Executable Source
SCHARF_S=Main.cpp
SCHARF_UT_S=unitTest/Main_UT.cpp

#Tasks
$(ODIR_C)/%.o: $(SDIR_C)/%.cpp $(DEPSC)
	@$(CC) $(CFLAGS) -c -o $@ $< $(DFLAGS) $(LIBS)
	@echo Building: $@

$(ODIR)/%.o: $(SDIR)/%.cpp $(OBJC) $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $< $(DFLAGS) $(LIBS)
	@echo Building: $@

#Build main executable
scharf: $(SCHARF_S) $(OBJC) $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $^ $(DFLAGS) $(LIBS)
	@echo Linking: $@

#Build unit test
scharfUT: $(SCHARF_UT_S) $(OBJC) $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $^ $(DFLAGS) $(LIBS)
	@echo Linking: $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ $(SDIR)/*~
	rm -f $(ODIR_C)/*.o *~ $(IDIR_C)/*~ $(SDIR_C)/*~
	rm -f $(UTDIR)/*.o *~
