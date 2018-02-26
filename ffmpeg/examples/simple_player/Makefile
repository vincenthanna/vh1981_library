CXXFLAGS = -g
TRACE_FLAGS =  -Wl,-v
TARGET = a.out
CXX = g++

OBJS = main.o
#LIBS = -lavutil -lavformat -lavcodec -lz -lavutil -lm -lswscale -lavfilter -lswresample -lavdevice -lrt -lpthread -ldl # linux
LIBS = -lavutil -lavformat -lavcodec -lz -lavutil -lm -lswscale -lavfilter -lswresample -lavdevice -lpthread -ldl # OSX

#-lavformat -lavcodec -lswscale -lavutil -lavfilter -lswresample -lavdevice -lpostproc -lao -lz -lx264 -llzma -lbz2 -lva -lrt

$(TARGET): $(OBJS)
	$(CXX) $(TRACE_FLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS) -I /usr/local/include
	
clean::
	rm -rf $(TARGET) $(OBJS)

.PHONY: clean
