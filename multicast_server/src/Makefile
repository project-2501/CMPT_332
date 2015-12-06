# Description: Makefile for Multicast Messaging server
#
# Authors: Taran and Kyle
# Class:   CMPT 332 - Fall 2015

# Directories to include in the build
LDLIBS = `pkg-config --libs glib-2.0` -pthread

# Compiler flags
CFLAGS = -std=c99 -g `pkg-config --cflags glib-2.0` -pedantic -Wall -Wextra \
		 -Wmissing-prototypes -Wno-sign-compare -Wno-unused-parameter \
		 -D_GNU_SOURCE 

RM = rm -f

# List of executable files to be built
EXE = multicast_server send_client recv_client

all: 
	${MAKE} ${EXE}


multicast_server: multicast_server.c listener_thread.c send_client_thread.c recv_client_thread.c
	${CC} ${CFLAGS} ${LDLIBS} -o multicast_server multicast_server.c listener_thread.c send_client_thread.c recv_client_thread.c

clean: 
	${RM} ${EXE} *.o *.a