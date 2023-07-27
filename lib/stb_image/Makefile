INC = ./include
SRC = ./src
LIB = ./lib

CC = gcc
CFLAGS = -Wall -g -O3 -I$(INC)

AR = ar
RM = rm

all: $(LIB)/libstb_image.a

$(LIB)/libstb_image.a: stb_image.o
	$(AR) -rcs $@ $^
	$(RM) $^

%.o: $(SRC)/%.c $(INC)/%.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) -f $(LIB)/*

