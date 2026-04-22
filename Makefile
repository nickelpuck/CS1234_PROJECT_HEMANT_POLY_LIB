CC      = gcc
CFLAGS  = -Wall -Wextra -g
LDFLAGS = -lm

LIB_SRCS = poly.c graph.c
LIB_OBJS = poly.o graph.o
LIB_A    = libpoly.a

MAIN_SRC = main.c
TARGET   = polydemo

all: $(LIB_A) $(TARGET)

poly.o: poly.c poly.h
	$(CC) $(CFLAGS) -c poly.c -o poly.o

graph.o: graph.c graph.h poly.h
	$(CC) $(CFLAGS) -c graph.c -o graph.o

$(LIB_A): $(LIB_OBJS)
	ar rcs $(LIB_A) $(LIB_OBJS)

$(TARGET): $(MAIN_SRC) $(LIB_A) poly.h graph.h
	$(CC) $(CFLAGS) $(MAIN_SRC) -L. -lpoly $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(LIB_OBJS) $(LIB_A) $(TARGET) poly_saved.txt graph_saved.txt

.PHONY: all clean
