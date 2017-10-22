 # SS Server Makefile

all: $(SS)

$(SS): SSserver.o
	$(COMPILER) -o $(SS) SSserver.o

SSserver.o: SSserver.c
	$(COMPILER) $(COMPILER_FLAGS) SSserver.c $(FLAGS)
 
clean:
	rm -f *~ *.o $(SS)