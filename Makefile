CC=gcc

CFLAGS=-Wall -Wextra -g3
LFLAGS=

OBJS=avl.o chrom.o conn.o ga-timer.o input.o main.o population.o selector.o simulation.o uhurricane-listener.o ushortid-server.o wnode.o sendtomote.o
DEPS=avl.h conn.h population.h simulation.h ushortid-server.h chrom.h input.h selector.h uhurricane-listener.h wnode.h sendtomote.h
LIBS=-lpthread -lm

BIN=svr

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(BIN)
