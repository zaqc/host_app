TARGET := host_app
CFLAGS := -g3 -Wall -I src/
PKG_NAME := pkg-config libftdi1 sdl2 SDL2_ttf libxml-2.0 SDL2_gfx zlib

GPP := g++

#OBJ = main.o obj/TrackBar.o obj/Label.o obj/Button.o obj/Window.o obj/AScanWnd.o obj/BScanWnd.o obj/Desktop.o

SRC := $(wildcard src/*.cpp)
SRC += $(wildcard src/hw/*.cpp)
HDR := $(patsubst %.cpp,%.h,$(SRC))
OBJ := main.o
OBJ += $(patsubst src/%.cpp,obj/%.o,$(SRC))
#SRC += main.cpp

all: $(TARGET)
	
#$(OBJ) : $(SRC) $(HDR)
obj/hw/%.o : src/hw/%.cpp $(HDR)
	@if [ ! -d obj/hw ]; then mkdir obj/hw; fi
	$(GPP) -c $< -o $@ $(CFLAGS) $(shell $(PKG_NAME) --cflags)
	
obj/%.o : src/%.cpp $(HDR)
	@if [ ! -d obj ]; then mkdir obj; fi
	$(GPP) -c $< -o $@ $(CFLAGS) $(shell $(PKG_NAME) --cflags)

#	g++ -c $(patsubst obj/%.o,src/%.cpp,$@) -o $@ $(CFLAGS) $(shell $(PKG_NAME) --cflags)

%.o : %.cpp $(HDR)
	@echo Compile Project [$(TARGET)]...
	$(GPP) -c $< -o $@ $(CFLAGS) $(shell $(PKG_NAME) --cflags)
	
#	
#obj/TrackBar.o: src/TrackBar.cpp $(HDR)
#	g++ -c src/TrackBar.cpp -o obj/TrackBar.o $(CFLAGS) $(shell $(PKG_NAME) --cflags)
#
#obj/Label.o: src/Label.cpp src/Label.h
#	g++ -c src/Label.cpp -o obj/Label.o $(CFLAGS) $(shell $(PKG_NAME) --cflags)
#	
#obj/Button.o: src/Button.cpp src/Button.h
#	g++ -c src/Button.cpp -o obj/Button.o $(CFLAGS) $(shell $(PKG_NAME) --cflags)
#	
#obj/Window.o: src/Window.cpp src/Window.h
#	g++ -c src/Window.cpp -o obj/Window.o $(CFLAGS) $(shell $(PKG_NAME) --cflags)
#	
#obj/AScanWnd.o: src/AScanWnd.cpp src/AScanWnd.h
#	g++ -c src/AScanWnd.cpp -o obj/AScanWnd.o $(CFLAGS) $(shell $(PKG_NAME) --cflags)
#	
#obj/BScanWnd.o: src/BScanWnd.cpp src/BScanWnd.h
#	g++ -c src/BScanWnd.cpp -o obj/BScanWnd.o $(CFLAGS) $(shell $(PKG_NAME) --cflags)
#	
#obj/Desktop.o: src/Desktop.cpp src/Desktop.h
#	g++ -c src/Desktop.cpp -o obj/Desktop.o $(CFLAGS) $(shell $(PKG_NAME) --cflags)
	
$(TARGET): $(OBJ)
	@echo Linking project...
	$(GPP) -o $(TARGET) $(OBJ) $(shell $(PKG_NAME) --libs) -lpthread
#	sshpass -p odroid scp $(TARGET) odroid@10.0.0.44:/home/odroid/work/host_app/
	
clean:
	rm -f *.o obj/*.o obj/hw/*.o
