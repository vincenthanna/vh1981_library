CXXFLAGS = -g -std=c++11
#TRACE_FLAGS =  -Wl,-v
TARGET = a.out
CXX = g++
AR = ar
RANLIB = ranlib

LIB_NAME = vh1981_basic
LIB_FULL_NAME = lib$(LIB_NAME).a

SRCS = exstring.cpp exlog.cpp exthread.cpp
OBJS = $(SRCS:%.cpp=%.o)

LIBS = -lgtest -lpthread

all : $(LIB_FULL_NAME)

$(LIB_FULL_NAME) : $(OBJS)
	$(AR) rcv $@ $(OBJS)
	$(RANLIB) $@
	echo $(OBJS)
	
clean::
	rm -rf $(LIB_FULL_NAME) $(OBJS)

.PHONY: clean
