# Description: Makefile for odd shell
#
# Authors: Taran and Kyle
# Class:   CMPT 332 - Fall 2015

# Directories to include in the build
OSH_INC_DIRS = lib
OSH_INC_LIBS = lib/util_functions.a
LDLIBS = ${OSH_INC_LIBS}

# Compiler flags
CFLAGS = -std=c99 -g -I${OSH_INC_DIRS} -pedantic -Wall -Wextra \
		 -Wmissing-prototypes -Wno-sign-compare -Wno-unused-parameter \
		 -D_GNU_SOURCE -lreadline

RM = rm -f

# List of executable files to be built
EXE = osh

all:
	cd ${OSH_INC_DIRS} && ${CC} -g -c *.c && ${AR} rs util_functions.a *.o; \
	cd .. && ${MAKE} ${EXE}

clean: 
	${RM} ${EXE} *.o; \
	cd lib && ${RM} *.o *.a

