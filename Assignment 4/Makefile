%.o: %.c
	cc -g -Wall -c $<

all: skel prime

SKEL_OBJS = skel.o
skel: $(SKEL_OBJS)
	cc $(SKEL_OBJS) -o $@

PRIME_OBJS = primeMain.o prime.o
prime: $(PRIME_OBJS)
	cc $(PRIME_OBJS) -o $@

clean:
	rm *.o
	rm skel prime