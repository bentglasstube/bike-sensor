CC=clang++
CFLAGS=-g --std=c++03 -Wall -Wextra
LDLIBS=`sdl2-config --cflags --libs`

EXECUTABLE=bike-display

all: $(EXECUTABLE)

$(EXECUTABLE): bike-display.cc
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

app: BikeStats.app

BikeStats.app: $(EXECUTABLE) launcher sprites.bmp Info.plist
	rm -rf BikeStats.app
	mkdir -p BikeStats.app/Contents/{MacOS,Frameworks/Resources}
	cp $(EXECUTABLE) BikeStats.app/Contents/MacOS/exec
	cp launcher BikeStats.app/Contents/MacOS/launcher
	cp sprites.bmp BikeStats.app/Contents/MacOS/sprites.bmp
	cp Info.plist BikeStats.app/Contents/Info.plist
	cp -R /Library/Frameworks/SDL2.framework BikeStats.app/Contents/Frameworks/SDL2.framework

clean:
	rm -rf $(EXECUTABLE)

.PHONY: all app run clean
