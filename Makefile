CC      = gcc
CFLAGS  = -Wall -Wextra -g
LDFLAGS = -lm

LIB_SRC = poly.c
LIB_OBJ = poly.o
LIB_A   = libpoly.a

MAIN_SRC = main.c
TARGET   = polydemo

all: $(LIB_A) $(TARGET)

$(LIB_OBJ): $(LIB_SRC) poly.h
	$(CC) $(CFLAGS) -c $(LIB_SRC) -o $(LIB_OBJ)

$(LIB_A): $(LIB_OBJ)
	ar rcs $(LIB_A) $(LIB_OBJ)

$(TARGET): $(MAIN_SRC) $(LIB_A) poly.h
	$(CC) $(CFLAGS) $(MAIN_SRC) -L. -lpoly $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(LIB_OBJ) $(LIB_A) $(TARGET) poly_saved.txt

.PHONY: all clean
