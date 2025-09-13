CC = gcc
CFLAGS = -std=c17 -pedantic -Wall -g3

ParseCirc: parse_circ.o
        ${CC} ${CFLAGS} -c -fPIC parse_circ.c -o parse_circ.o $@ $^
        ${CC} ${CFLAGS} -shared -o libparse_circ.so parse_circ.o $@ $^
        ${CC} ${CFLAGS} erase_runner.c -L. -lparse_circ -o ParseCirc $@ $^

clean:
        rm -f *.o *.so ParseCirc