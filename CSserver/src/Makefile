# CS Server Makefile	
 
all: $(CS)

$(CS): CSserver.o Files.o
	$(COMPILER) -o $(CS) CSserver.o Files.o

Files.o: Files.c Files.h
	$(COMPILER) $(COMPILER_FLAGS) Files.c $(FLAGS)
	
CSserver.o: CSserver.c Files.h
	$(COMPILER) $(COMPILER_FLAGS) CSserver.c $(FLAGS)

clean:
	rm -f *~ *.o $(CS)