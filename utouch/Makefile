TARGET:=utouch

CXXFLAGS:=-g3 -Wall -I/usr/include -fmessage-length=0 -std=c++11

PKG:=egl glesv2 x11 libpng libftdi

CC=g++

INC_DIR := -I../DataFile -I../CartView -I../DataCacher -I../Misc -I../Graphics -I../MediaManager
HDR:=$(wildcard *.h)
HDR += $(wildcard ../DataCacher/*.h)
HDR += $(wildcard ../CartView/*.h)
HDR += $(wildcard ../DataFile/*.h)
HDR += $(wildcard ../Misc/*.h)

SRC:=$(wildcard *.cpp)
SRC += $(wildcard ../CartView/*.cpp)
SRC += $(wildcard ../Misc/*.cpp)
SRC += $(wildcard ../MediaManager/*.cpp)

SRC := $(filter-out ../CartView/ChanelDeffects.cpp, $(SRC))
SRC := $(filter-out ../CartView/DeffectsSample.cpp, $(SRC))
SRC := $(filter-out ../MediaManager/JavaAssetManager.cpp, $(SRC))

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
	$(CC) $(CXXFLAGS) $(INC_DIR) -c $< -o $@;	

$(TARGET): $(OBJS)
	@echo "Linking project ["$(TARGET)"] with FLAGS="$(LDFLAGS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) 

clean:
	rm -f *.o $(TARGET)
