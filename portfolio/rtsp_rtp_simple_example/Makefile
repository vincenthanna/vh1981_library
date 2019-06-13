ifeq ($(TARGET), Debug)
CXXFLAGS = -g -std=c++11
TARGET = ad.out
UNITTEST = unittestd
endif

ifeq ($(TARGET), Release)
CXXFLAGS = -std=c++11
TARGET = a.out
UNITTEST = unittest
endif


CXX = g++
AR = ar
RANLIB = ranlib
CXXFLAGS += -D__STDC_CONSTANT_MACROS

LIBS = -lvh1981_basic

SUBDIRS += ../../library/basic

SRCS_SUB = CRtspSession.cpp CStreamer.cpp JPEGSamples.cpp RtspParser.cpp RtspTypes.cpp Packet.cpp Session.cpp RTSPTestServer.cpp Mule.cpp
SRCS = $(SRCS_SUB) RTSPServerTest.cpp
OBJS_SUB = $(SRCS_SUB:%.cpp=%.o)
OBJS = $(SRCS:%.cpp=%.o)

LIBS += -lpthread -ljrtp -lboost_regex

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

$(UNITTEST) : unittest.o $(SRCS_SUB)
	$(CXX) $(TRACE_FLAGS) $(CXXFLAGS) -o $(UNITTEST) $(SRCS_SUB) unittest.o $(LFLAGS_ALL) $(LIBS) -lgtest -lpthread -I /usr/local/include 
	
clean::
	rm -rf $(TARGET) $(OBJS) $(UNITTEST) unittest.o

.PHONY: clean
