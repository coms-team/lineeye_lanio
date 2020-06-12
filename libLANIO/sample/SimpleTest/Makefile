LE_LINK = -lLANIO    ### LINEEYE LANIO lib ###

PROG = libLANIO_sample
OBJS = libLANIO_sample.o

all : $(PROG)
$(PROG) : $(OBJS)
	gcc -o $@ $^ $(LE_LINK)
.c.o :
	gcc -c $<
clean :
	rm -f $(PROG) $(OBJS)
