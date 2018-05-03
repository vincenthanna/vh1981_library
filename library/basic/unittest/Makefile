CXXFLAGS = -g -std=c++11 -I ../
#TRACE_FLAGS =  -Wl,-v
TARGET = a.out
CXX = g++

OBJS = ../exstring.o ../exlog.o ../exthread.o ../exmutex.o unittest.o

LIBS = -lgtest -lpthread

$(TARGET): $(OBJS)
	$(CXX) $(TRACE_FLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS) -I /usr/local/include
	
clean::
	rm -rf $(TARGET) $(OBJS)

.PHONY: clean
