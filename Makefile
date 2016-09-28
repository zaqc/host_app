TARGET=host_app
CFLAGS=-g3 -Wall -I src/

OBJ = main.o obj/TrackBar.o obj/Label.o obj/Button.o obj/Window.o obj/AScanWnd.o obj/BScanWnd.o obj/Desktop.o

all: $(TARGET)

main.o: main.cpp
	@echo compile main.cpp
	g++ -c main.cpp $(CFLAGS) $(shell pkg-config sdl2 SDL2_ttf SDL2_gfx --cflags)
	
obj/TrackBar.o: src/TrackBar.cpp src/TrackBar.h
	g++ -c src/TrackBar.cpp -o obj/TrackBar.o $(CFLAGS) $(shell pkg-config sdl2 --cflags)

obj/Label.o: src/Label.cpp src/Label.h
	g++ -c src/Label.cpp -o obj/Label.o $(CFLAGS) $(shell pkg-config sdl2 SDL2_ttf --cflags)
	
obj/Button.o: src/Button.cpp src/Button.h
	g++ -c src/Button.cpp -o obj/Button.o $(CFLAGS) $(shell pkg-config sdl2 SDL2_ttf --cflags)
	
obj/Window.o: src/Window.cpp src/Window.h
	g++ -c src/Window.cpp -o obj/Window.o $(CFLAGS) $(shell pkg-config sdl2 SDL2_ttf --cflags)
	
obj/AScanWnd.o: src/AScanWnd.cpp src/AScanWnd.h
	g++ -c src/AScanWnd.cpp -o obj/AScanWnd.o $(CFLAGS) $(shell pkg-config sdl2 SDL2_ttf --cflags)
	
obj/BScanWnd.o: src/BScanWnd.cpp src/BScanWnd.h
	g++ -c src/BScanWnd.cpp -o obj/BScanWnd.o $(CFLAGS) $(shell pkg-config sdl2 SDL2_ttf --cflags)
	
obj/Desktop.o: src/Desktop.cpp src/Desktop.h
	g++ -c src/Desktop.cpp -o obj/Desktop.o $(CFLAGS) $(shell pkg-config sdl2 SDL2_ttf --cflags)
	
$(TARGET): $(OBJ)
	@echo Linking project...
	g++ -o $(TARGET) $(OBJ) $(shell pkg-config sdl2 SDL2_ttf SDL2_gfx --libs)
	
clean:
	rm -f *.o obj/*.o