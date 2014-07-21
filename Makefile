CC=gcc
COPT=-Wall

all: main.o nids.o hash.o stream.o http_parser.o output.o
	$(CC) -o http_capture main.o nids.o hash.o stream.o http_parser.o output.o -lnids -lpcap -lglib-2.0 -lgthread-2.0 -lpthread -ljansson

nids.o: nids.c nids.h
	$(CC) $(COPT) -c nids.c

nidsdummy.o: nidsdummy.c nids.h
	$(CC) $(COPT) -c nidsdummy.c

hash.o: hash.c hash.h config.h
	$(CC) $(COPT) -c hash.c

stream.o: stream.c stream.h config.h
	$(CC) $(COPT) -c stream.c

http_parser.o: http_parser.c http_parser.h config.h
	$(CC) $(COPT) -c http_parser.c

output.o: output.c output.h
	$(CC) $(COPT) -c output.c

main.o: main.c config.h
	$(CC) $(COPT) -c main.c

clean:
	rm -f *.o *~ httpsniff
