##########################################################
##                 Created  by  xiaohai                 ##
##                 Telphone : 18501781924               ##
##            E-mail : jinxiaohai@sinap.ac.cn           ##
##            E-mail : xiaohaijin@outlook.com           ##
##   Address : Shanghai Institute of Applied Physics    ##
##########################################################

EXE = analysis.exe
OBJ = analysis.o V0PicoDst.o
ROOTLIBS     =   $(shell root-config --libs)
ROOTGLIBS    =   $(shell root-config --glibs)

INCDIR = -I$(ROOTSYS)/include
LIBDIR = -L$(ROOTSYS)/lib
BOOSTINCDIR = -I$(BOOSTSYS)/include

CC = g++ -std=c++11
FLAGS = -Wall -g $(INCDIR) $(LIBDIR) $(BOOSTINCDIR)
COMPILE = $(CC) $(FLAGS) -c

$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(ROOTLIBS) $(ROOTGLIBS)

%.o:%.cpp
	$(COMPILE) $< -o $@

.PHONY:clean
clean:
	$(RM) -r $(OBJ) $(EXE)
