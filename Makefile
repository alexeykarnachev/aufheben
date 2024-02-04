CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -L./deps/lib
LIBS = -lraylib -lm -lpthread -ldl
INCLUDES = -I./deps/include


aufheben:
	$(CC) $(CFLAGS) $(INCLUDES) -o build/$@ \
		./bin/$@.c \
		./src/shader.c \
		./src/camera.c \
		./src/lights.c \
		$(LDFLAGS) $(LIBS)

