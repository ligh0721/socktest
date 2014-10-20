ifeq ($(ARM), 1)
PLATFORM = arm
CXX = /home/thunderliu/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-g++
AR = /home/thunderliu/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-ar
RANLIB = /home/thunderliu/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-ranlib
NM = /home/thunderliu/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-nm
CFLAGS = 
CXXFLAGS = -I/home/thunderliu/CodeSourcery/Sourcery_G++_Lite/arm-none-linux-gnueabi/libc/usr/include -I/home/thunderliu/CodeSourcery/Sourcery_G++_Lite/arm-none-linux-gnueabi/include/c++/4.5.2
#LDFLAGS = -Wl,-dynamic-linker=/data/ts/lib/ld-linux.so.3 -L/home/thunderliu/CodeSourcery/Sourcery_G++_Lite/arm-none-linux-gnueabi/libc/lib -L/home/thunderliu/CodeSourcery/Sourcery_G++_Lite/arm-none-linux-gnueabi/libc/usr/lib
LDFLAGS = -L/home/thunderliu/CodeSourcery/Sourcery_G++_Lite/arm-none-linux-gnueabi/libc/lib -L/home/thunderliu/CodeSourcery/Sourcery_G++_Lite/arm-none-linux-gnueabi/libc/usr/lib 
BINDIR = ../../../bin_arm

else
PLATFORM = x86
CXX = g++
AR = ar
RANLIB = ranlib
NM = nm
CFLAGS = -g
CXXFLAGS = 
LDFLAGS = 
ifeq ($(STATIC), 1)
LDFLAGS += -static
endif
BINDIR = ../../../bin
endif

LIB_ROOT = ../../../src/lib/$(PLATFORM)/usr/local
INCLUDE = $(LIB_ROOT)/include
LIB = $(LIB_ROOT)/lib

CXXFLAGS += -I$(INCLUDE)
#LDFLAGS +=  -L$(LIB) -ldl -lncurses $(LIB)/libreadline.a
#LDFLAGS +=  -ldl -lncurses $(LIB)/libreadline.a
LDFLAGS +=  -ldl -lncurses -lreadline -lsocktest
LIBLDFLAGS = -fPIC -shared -lpthread

SOCKTESTLIB = /usr/local/lib/libsocktest.a
PLUGINLIB = $(BINDIR)/file.slb
BIN = $(PLUGINLIB)

OBJ = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
#COMMOBJ = stdinc.o Socket.o CmdParser.o Section.o CommCmd.o
COMMOBJ += $(TESTOBJ)

$(BIN):  file.o
	$(CXX) $^ -o $@ $(CFLAGS) $(LDFLAGS) $(LIBLDFLAGS)
#	$(NM) $@
	
%.o: %.cpp
	$(CXX) $< -c -o $@ $(CFLAGS) $(CXXFLAGS)
	
.PHONY: all clean mkdir
all: $(BIN)
	
clean:
	-rm $(OBJ) $(BIN)
	
mkdir:
	-mkdir -p $(BINDIR)
