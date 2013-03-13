#
#  There exist several targets which are by default empty and which can be 
#  used for execution of your targets. These targets are usually executed 
#  before and after some main targets.
#

CC=gcc
LDADD=-lnivel1 -lpthread

all:	envia recibe

envia: envia.o
	$(CC) -o envia envia.o $(LDADD)
	
recibe: recibe.o
	$(CC) -o recibe recibe.o $(LDADD)

clean:
	rm -fr core *.o envia recibe
