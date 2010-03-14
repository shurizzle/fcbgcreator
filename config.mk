VERSION = 0.3

PREFIX = /usr/local

X11LIB = -lX11
GDLIB = -lgd

INCS = -I/usr/include
LIBS = ${X11LIB} ${GDLIB}

CFLAGS = -Wall -Wextra -ansi -pedantic -pedantic-errors ${INCS}
LDFLAGS = -s ${LIBS}

CC = gcc
