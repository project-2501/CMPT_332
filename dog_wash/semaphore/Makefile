# Description: Makefile for dogwash problem
#
# Authors: Taran and Kyle
# Class:   CMPT 332 - Fall 2015

# Directories to include in the build
OSH_INC_DIRS = 
OSH_INC_LIBS =  
LDLIBS = ${OSH_INC_LIBS}

# Compiler flags
CFLAGS = -std=c99 -g -I${OSH_INC_DIRS} -pedantic -Wall -Wextra \
		 -Wmissing-prototypes -Wno-sign-compare -Wno-unused-parameter \
		 -D_GNU_SOURCE -pthread

RM = rm -f

# List of executable files to be built
EXE = test_dogwash regression_test_dogwash

all: 
	${MAKE} ${EXE}

test_dogwash: test_dogwash.c dogwashsynch.c
	${CC} ${CFLAGS} -o test_dogwash test_dogwash.c dogwashsynch.c

regression_test_dogwash: regression_test_dogwash.c dogwashsynch.c
	${CC} ${CFLAGS} -o regression_test_dogwash regression_test_dogwash.c dogwashsynch.c

clean: 
	${RM} ${EXE} *.o *.a

