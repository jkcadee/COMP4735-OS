%.o: %.c
	cc -g -Wall -c $<

all: vmem

VMEM_OBJ = vmem.o
vmem: $(VMEM_OBJ)
	cc $(VMEM_OBJ) -o $@

clean:
	rm *.o
	rm vmem