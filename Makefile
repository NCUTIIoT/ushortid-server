CC=gcc

CFLAGS=-Wall -Wextra -g3
LFLAGS=

OBJS=avl.o main.o ushortid-server.o
DEPS=avl.h ushortid-server.h
LIBS=-lpthread

BIN=svr

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(BIN)
