# Description: Makefile for memory allocator 
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
		 -D_GNU_SOURCE -lreadline

RM = rm -f

# List of executable files to be built
EXE = test_coalesce test_next

all: 
	${MAKE} ${EXE}

test_coalesce: test_coalesce.c mem_alloc.c
	${CC} ${CFLAGS} -o test_coalesce test_coalesce.c mem_alloc.c

test_next: test_next.c mem_alloc.c
	${CC} ${CFLAGS} -o test_next test_next.c mem_alloc.c

clean: 
	${RM} ${EXE} *.o

