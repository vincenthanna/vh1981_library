CXXFLAGS = -g -std=c++11
#TRACE_FLAGS =  -Wl,-v
TARGET = a.out
CXX = g++

CXXFLAGS += -D__STDC_CONSTANT_MACROS

LFLAGS_ALL := -L ../../library/basic/

LIBS = -lvh1981_basic

OBJS = Player.o MotionSearch.o Main.o
LIBS += -lavutil -lavformat -lavcodec -lz -lavutil -lm -lswscale -lavfilter -lswresample -lavdevice -lrt -lpthread -ldl # linux
#LIBS = -lavutil -lavformat -lavcodec -lz -lavutil -lm -lswscale -lavfilter -lswresample -lavdevice -lpthread -ldl # OSX

$(TARGET): $(OBJS)
	$(CXX) $(TRACE_FLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LFLAGS_ALL) $(LIBS) -I /usr/local/include 
	
clean::
	rm -rf $(TARGET) $(OBJS)

.PHONY: clean
