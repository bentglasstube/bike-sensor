CC=clang++
CFLAGS=-g --std=c++03 -Wall -Wextra
LDLIBS=`sdl2-config --cflags --libs`

EXECUTABLE=bike-display

all: $(EXECUTABLE)

$(EXECUTABLE): bike-display.cc
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -rf $(EXECUTABLE)

.PHONY: all run clean
