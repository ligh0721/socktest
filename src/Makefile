CXX = $(TOOLCHAIN)g++
AR = $(TOOLCHAIN)ar
RANLIB = $(TOOLCHAIN)ranlib
NM = $(TOOLCHAIN)nm
CFLAGS = -g -fPIC
CXXFLAGS = 
LDFLAGS = 
ifeq ($(STATIC), 1)
LDFLAGS += -static
endif
BINDIR = ../bin

LIB_ROOT = .
INCLUDE = $(LIB_ROOT)/include
LIB = $(LIB_ROOT)/lib

CXXFLAGS += -I$(INCLUDE)
#LDFLAGS +=  -L$(LIB) -ldl -lncurses $(LIB)/libreadline.a
LDFLAGS +=  -ldl -lncurses -lreadline -lpthread
LIBLDFLAGS = -fPIC -shared -lpthread
PROTOC = protoc


SOCKTEST = $(BINDIR)/socktest
SOCKTESTLIB = $(BINDIR)/libsocktest.a
PLUGINLIB = $(BINDIR)/sample.slb
BIN = $(SOCKTEST) $(SOCKTESTLIB) $(patsubst %.a, %.so, $(SOCKTESTLIB))

OBJ = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
COMMOBJ = socktest.o Socket.o Section.o CmdParser.o CommCmd.o
TESTOBJ = 
COMMOBJ += $(TESTOBJ)
PB = $(wildcard *.pb.*)

	
.PHONY: all clean mkdir install
all: $(BIN)
	
clean:
	-rm -rf $(OBJ) $(BIN) $(PB) $(BINDIR)/socktest.tar.bz2
	
mkdir:
	-ln -s /usr/local/lib/libreadline.a $(LIB)/libreadline.a
	-mkdir -p $(BINDIR)

PREFIX = ./release

pack: all
	-mkdir -p $(PREFIX)/include/socktest
	-mkdir -p $(PREFIX)/lib/socktest
	-mkdir -p $(PREFIX)/bin
	-cp $(SOCKTEST) sendf.st recvf.st $(PREFIX)/bin/
	-cp $(BINDIR)/*.slb $(PREFIX)/lib/socktest/
	-cp $(SOCKTESTLIB) $(PREFIX)/lib/
	-cp $(patsubst %.a, %.so, $(SOCKTESTLIB)) $(PREFIX)/lib/
	-cp socktest.h $(PREFIX)/include/socktest/
	-cp Socket.h $(PREFIX)/include/socktest/
	-cp Section.h $(PREFIX)/include/socktest/
	-cp CmdParser.h $(PREFIX)/include/socktest/
	-cd $(PREFIX) && tar jcf socktest.tar.bz2 bin include lib

$(SOCKTEST): $(COMMOBJ)
	$(CXX) $^ -o $@ $(CFLAGS) $(LDFLAGS)
	
$(SOCKTESTLIB): socktest.o Socket.o Section.o CmdParser.o
	$(AR) -cr $@ $^
	$(RANLIB) $@
#	$(NM) $@
	$(CXX) $^ -o $(patsubst %.a, %.so, $@) $(CFLAGS) $(LDFLAGS) $(LIBLDFLAGS)
	
sample.cpp: MsgBusBody.pb.cpp
	
$(PLUGINLIB): sample.o MsgBusBody.pb.o $(SOCKTESTLIB)
	$(CXX) $^ -o $@ $(CFLAGS) $(LDFLAGS) $(LIBLDFLAGS)
#	$(NM) $@
	

%.pb.cpp: %.proto
	$(PROTOC) $< --cpp_out=./
	mv $(@:.pb.cpp=.pb.cc) $@


%.o: %.cpp
	$(CXX) $< -c -o $@ $(CFLAGS) $(CXXFLAGS)
	
	
