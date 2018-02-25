CXXFLAGS = -g 
TARGET = a.out
CXX = g++

OBJS = running.o
LIBS = -lavutil -lavformat -lavcodec -lz -lavutil -lm

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)
	
clean::
	rm -rf $(TARGET) $(OBJS)

.PHONY: clean