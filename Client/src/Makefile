# Client Makefile
		
all: $(USER)

$(USER): teste.o Client.o
	$(COMPILER) -o $(USER) Client.o teste.o

teste.o: teste.c header.h
	$(COMPILER) $(COMPILER_FLAGS) teste.c $(FLAGS)

Client.o: Client.c header.h
	$(COMPILER) $(COMPILER_FLAGS) Client.c $(FLAGS)

clean:
	-rm -f *.o *~ $(USER)
