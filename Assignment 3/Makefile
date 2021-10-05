CFLAGS = -Wall -pthread
files = primeMain talk threads thread_race

all: $(files)

$(files): %: %.c
	$(CC) $(CFLAGS) -o $@ $<