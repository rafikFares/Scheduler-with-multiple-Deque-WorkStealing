CC = gcc
CFLAGS = -g -W -Wall
LDLIBS =  -lpthread

SRC = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJ = $(SRC:.c=.o)
AOUT = run
 
all : $(AOUT) 

run: $(HEADERS)
	$(CC) $(SRC) $(CFLAGS) $(LDLIBS) -o $@ $^
clean:
	@rm $(AOUT)
mrproper : clean
	@rm $(AOUT)