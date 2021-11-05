%.o: %.c
	cc -g -Wall -c $<

all: skel prime producer

SKEL_OBJS = skel.o
skel: $(SKEL_OBJS)
	cc $(SKEL_OBJS) -o $@

PRIME_OBJS = primeMain.o prime.o
prime: $(PRIME_OBJS)
	cc $(PRIME_OBJS) -o $@

PRODUCER_OBJS = producer.o
producer: $(PRODUCER_OBJS)
	cc -pthread $(PRODUCER_OBJS) -o $@

clean:
	rm *.o
	rm skel prime producer