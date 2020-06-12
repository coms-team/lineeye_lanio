LE_LINK = -lLANIO ### LINEEYE LANIO lib ###

WX_LINK = $(shell wx-config --libs)
WX_CXXFLAG = $(shell wx-config --cxxflags) -Wall

PROG = LAPCsample
OBJS = LAPCsample.o LANIOList.o LANIOPanel.o

all : $(PROG)
$(PROG) : $(OBJS)
	g++ -o $@ $^ $(WX_LINK) $(LE_LINK)
LEPC10L.o : Makefile LAPCsample.h LANIOPanel.h LANIOList.h defines.h
.cc.o :
	g++ $(WX_CXXFLAG) -c $<
clean :
	rm -f $(PROG) $(OBJS)
