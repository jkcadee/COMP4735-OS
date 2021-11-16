%.o: %.c
	cc -g -Wall -c $<

all: sched

SCHED_OBJS = sched.o
sched: $(SCHED_OBJS)
	cc -pthread $(SCHED_OBJS) -o $@

clean:
	rm *.o
	rm sched