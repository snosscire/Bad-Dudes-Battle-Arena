CC=gcc -g -ansi -pedantic
FLAGS=-Wall
LIBS=-lSDL -lSDL_image -lenet -lmjson
SOURCES=main.c \
	client.c \
	server.c \
	list.c \
	time.c \
	graphics.c \
	input.c \
	network.c \
	map.c \
	game-object.c \
	sprite.c \
	console.c \
	string_tobias.c \
	font.c \
	client-callbacks.c
OBJECTS=$(SOURCES:.c=.o)
PROGRAM=game


all: $(OBJECTS) $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(PROGRAM) $(LIBS)

%.o: %.c
	$(CC) -o $@ $(FLAGS) -c $<

clean:
	rm -rf *.o $(PROGRAM)

