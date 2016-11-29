CXX=g++
LDFLAGS=-lsimlib -lm

SRCS=main.cc
TARGET=ims

all:
	${CXX} -std=c++11 -Wall ${SRCS} -o ${TARGET} ${LDFLAGS}

run:
	./${TARGET}

clean:
	rm -f ${RESULT} *.o *.a
