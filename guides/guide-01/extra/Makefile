CC=gcc
CFLAGS= -Wall -g
DEPS = metaindex.h
OBJ = main.o metaindex.o

LIBS=`pkg-config --cflags --libs glib-2.0` -lpthread

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o main