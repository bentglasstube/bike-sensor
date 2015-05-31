CC=clang
CFLAGS=-O -Wall -Wextra
LDLIBS=-lm `sdl2-config --cflags --libs`

EXECUTABLE=bike-display

all: $(EXECUTABLE)

$(EXECUTABLE): bike-display.cc
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

app: BikeStats.app

BikeStats.app: $(EXECUTABLE) launcher sprites.bmp Info.plist icon.icns
	rm -rf BikeStats.app
	mkdir -p BikeStats.app/Contents/{MacOS,Frameworks,Resources}
	cp $(EXECUTABLE) BikeStats.app/Contents/MacOS/exec
	cp launcher BikeStats.app/Contents/MacOS/launcher
	cp sprites.bmp BikeStats.app/Contents/MacOS/sprites.bmp
	cp Info.plist BikeStats.app/Contents/Info.plist
	cp -R /Library/Frameworks/SDL2.framework BikeStats.app/Contents/Frameworks/SDL2.framework
	cp icon.icns BikeStats.app/Contents/Resources/icon.icns

icon.icns: icon.iconset/*
	iconutil -c icns -o $@ icon.iconset

clean:
	rm -rf $(EXECUTABLE) BikeStats.app icon.icns

.PHONY: all app run clean
