# MAIN MAKEFILE
# Makefile build all 3 programs

NUMERO_GRUPO = 13

# Header path relatively to src directory.
HEADERS_PATH = ../headers

COMPILER = gcc
COMPILER_FLAGS = -c -g -Wall
FLAGS = -I$(HEADERS_PATH)
# VPATH it's where look to file files gcc need to compile.
VPATH = ../headers

# NAMES to the executable names:
USER = user
CS = CS
SS = SS

# Exporting all variables to the sub-makes.
export

sub:
	cd Client/src && \
	echo ------------------------ Starting $(USER) Compilation ----------------------- &&\
	$(MAKE) && \
	echo ------------------------ Ending $(USER) Compilation ----------------------- &&\
	mv $(USER) ../.. && \
	cd ../../CSserver/src && \
	echo ------------------------ Starting $(CS) Compilation ----------------------- &&\
	$(MAKE) && \
	echo ------------------------ Ending $(CS) Compilation ----------------------- &&\
	mv $(CS) ../..  && \
	cd ../../SSserver/src && \
	echo ------------------------ Starting $(SS) Compilation ----------------------- &&\
	$(MAKE) && \
	echo ------------------------ Ending $(SS) Compilation ----------------------- &&\
	mv $(SS) ../..

zip:
	zip  proj$(NUMERO_GRUPO) *

clean:
	rm -f *~ $(USER) $(SS) $(CS) && \
	cd Client/src && \
	make clean && \
	cd ../../CSserver/src && \
	make clean && \
	cd ../../SSserver/src && \
	make clean