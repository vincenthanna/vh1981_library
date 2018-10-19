CXXFLAGS = -g -std=c++11
#TRACE_FLAGS =  -Wl,-v
TARGET = a.out
CXX = g++

CXXFLAGS += -D__STDC_CONSTANT_MACROS

LIBS = -lvh1981_basic

SUBDIRS += library/basic

OBJS = Player.o main.o SDLDisplay.o
LIBS += -lavutil -lavformat -lavcodec -lz -lavutil -lm -lswscale -lavfilter -lswresample -lavdevice -lrt -lpthread -ldl -lSDL2 # linux
#LIBS = -lavutil -lavformat -lavcodec -lz -lavutil -lm -lswscale -lavfilter -lswresample -lavdevice -lpthread -ldl # OSX

all : subdirs

subdirs:
	@for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir;\
	done 

LFLAGS_ALL := -L library/basic/

all : subdirs $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(TRACE_FLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LFLAGS_ALL) $(LIBS) -I /usr/local/include 
	
clean::
	rm -rf $(TARGET) $(OBJS)

.PHONY: clean
