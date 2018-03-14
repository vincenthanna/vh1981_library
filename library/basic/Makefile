CXXFLAGS = -g -std=c++11
#TRACE_FLAGS =  -Wl,-v
TARGET = a.out
CXX = g++

OBJS = exstring.o main.o

$(TARGET): $(OBJS)
	$(CXX) $(TRACE_FLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS) -I /usr/local/include
	
clean::
	rm -rf $(TARGET) $(OBJS)

.PHONY: clean
