TARGET_OS = Linux

INC = ./include
SRC = ./src
LIB = ./lib

CFLAGS = -Wall -g -O3 -I$(INC)

ifeq ($(TARGET_OS),Windows)
	CC = x86_64-w64-mingw32-gcc
	LIBNAME = $(LIB)/libstb_image-mingw32.a
else
	CC = gcc
	LIBNAME = $(LIB)/libstb_image.a
endif

AR = ar
RM = rm

all: $(LIBNAME)

$(LIBNAME): stb_image.o
	$(AR) -rcs $@ $^
	$(RM) $^

%.o: $(SRC)/%.c $(INC)/%.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) -f $(LIB)/*

