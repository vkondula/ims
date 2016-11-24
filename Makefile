CXX=g++
LDFLAGS=-lsimlib -lm

SRCS=main.cc
TARGET=ims

all:
	${CXX} ${SRCS} -o ${TARGET} ${LDFLAGS}

run:
	./${TARGET}

clean:
	rm -f ${RESULT} *.o *.a
