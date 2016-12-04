CXX=g++
LDFLAGS=-lsimlib -lm

SRCS=main.cpp group.cpp waiter.cpp kitchen.cpp stats.cpp external_lib/json.hpp
TARGET=ims

all:
	${CXX} -std=c++11 -Wall ${SRCS} -o ${TARGET} ${LDFLAGS}

run:
	./${TARGET}

clean:
	rm -f ${RESULT} *.o *.a
