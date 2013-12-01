CC = g++
C_ARGS = -m64 -g -Wall -DMFP_EPOLL -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE

LIB = libsrv_framework.a 
OBJS = srv_framework.o utils.o

EXE = echo_server

.cpp.o:
	$(CC) $(C_ARGS) -DSYSEPOLL -c $^

all: $(LIB)


echo_server : echo_server.cpp
	$(CC) $(C_ARGS) -o $@ $^ $(LIB)

libsrv_framework.a: srv_framework.h $(OBJS)
	ar -rs $@ $(OBJS)

clean:
	rm -f *.o $(LIB)
