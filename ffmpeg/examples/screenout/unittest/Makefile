CXXFLAGS = -g -std=c++11 -I ../
#TRACE_FLAGS =  -Wl,-v
TARGET = a.out
CXX = g++

CXXFLAGS += -D__STDC_CONSTANT_MACROS

LIBS = -lvh1981_basic

SUBDIRS += library/basic

OBJS = unittest.o ../StreamQueue.o
LIBS += -lgtest -lpthread

LFLAGS_ALL := -L library/basic/

$(TARGET): $(OBJS)
	$(CXX) $(TRACE_FLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LFLAGS_ALL) $(LIBS) -I /usr/local/include
	
clean::
	rm -rf $(TARGET) $(OBJS)

.PHONY: clean
