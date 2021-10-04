files = hello hello1 hello2 count count1 count2

all: $(files)

$(files): %: %.c
	$(CC) $(CFLAGS) -o $@ $<
