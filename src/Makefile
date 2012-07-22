# Arbitrator Makefile
#
# Requirements:
#   - pthreads
#   - unix signals
#   - unix sockets


all: arbitrator


# Defining a suffix rule for .c -> .o
.c.o:
	gcc -g -c -o $@ $<


#
# Core Files
#
SRC_CORE = main.c                               \
           management.c                         \
           prompt.c                             \
           algorithms.c                         \
           services.c                           \
           nodes.c                              \
           connections.c
OBJ_CORE = ${SRC_CORE:.c=.o}


#
# Library Files
#
SRC_LIB = lib/wsocket.c                         \
          lib/hash.c
OBJ_LIB = ${SRC_LIB:.c=.o}


#
# Algorithm Files
#
SRC_ALGORITHMS = algorithms/roundrobin.c        \
                 algorithms/ratio.c             \
                 algorithms/dynamicratio.c      \
                 algorithms/leastconnections.c
OBJ_ALGORITHMS = ${SRC_ALGORITHMS:.c=.o}


arbitrator: $(OBJ_CORE) $(OBJ_LIB) $(OBJ_ALGORITHMS)
	gcc -Wall $(OBJ_CORE) $(OBJ_LIB) $(OBJ_ALGORITHMS) -pthread -o arbitrator


debug:
	make all DEBUG=1


clean:
	rm -rf *.o algorithms/*.o lib/*.o arbitrator
