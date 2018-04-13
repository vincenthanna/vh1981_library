CXXFLAGS = -g -std=c++11
#TRACE_FLAGS =  -Wl,-v
TARGET = a.out
CXX = g++
AR = ar
RANLIB = ranlib
CXXFLAGS += -D__STDC_CONSTANT_MACROS

LIBS = -lvh1981_basic

SUBDIRS += ../../library/basic

SRCS = CRtspSession.cpp CStreamer.cpp JPEGSamples.cpp RTSPTestServer.cpp
OBJS = $(SRCS:%.cpp=%.o)

LIBS += -lpthread -ljrtp

all : subdirs

subdirs:
	@for dir in $(SUBDIRS); do\
		$(MAKE) -C $$dir;\
	done 

LFLAGS_ALL := -L ../../library/basic/
LFLAGS_ALL += -L ../../library/jrtplib/src/

all : subdirs $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(TRACE_FLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LFLAGS_ALL) $(LIBS) -I /usr/local/include 
	
clean::
	rm -rf $(TARGET) $(OBJS)

.PHONY: clean
