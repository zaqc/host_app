TARGET:=utouch

CXXFLAGS:=-g3 -Wall -I/usr/include -fmessage-length=0

PKG:=egl glesv2 x11 libpng libftdi1 freetype2

CC=g++

HDR:=$(wildcard *.h)

SRC:=$(wildcard *.cpp)

OBJS:=$(patsubst %.cpp, %.o, $(SRC))

LIBS=

all: chk

chk:
	@echo "Compile project: "$(TARGET)
ifneq ($(shell if pkg-config --exists $(PKG); then echo "yes"; fi;),yes)
	@echo "[packages dependence] $(PKG)"
else
CXXFLAGS+=$(shell pkg-config $(PKG) --cflags)  
LDFLAGS:=$(shell pkg-config $(PKG) --libs)
LDFLAGS+= -lpthread
all: $(TARGET)
endif

%.o: %.cpp $(HDR)
	$(CC) $(CXXFLAGS) -c $< -o $@;	

$(TARGET): $(OBJS)
	@echo "Linking project ["$(TARGET)"] with FLAGS="$(LDFLAGS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) 

clean:
	rm -f *.o $(TARGET)
