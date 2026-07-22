CXX = clang++
include ../../platform.mk
BIN_DIR = ../lib
BIN = $(BIN_DIR)/libgame.a
SHARED_SRC = ../../../Lead-Shared-Source

CFLAGS = -m64 -g -O1 -pipe -frtti -fexceptions -pthread -D_THREAD_SAFE -fno-stack-protector -fno-strict-aliasing -I../include -I../../libthecore/include -I$(SHARED_SRC) $(PLATFORM_CFLAGS) $(PLATFORM_CXXFLAGS)

SRCS = $(wildcard *.cc)
OBJFILES = $(SRCS:.cc=.o)

default: $(BIN)

$(BIN): $(OBJFILES)
	mkdir -p $(BIN_DIR)
	ar cru $(BIN) $(OBJFILES)
	ranlib $(BIN)

clean:
	rm -f *.o $(BIN)

%.o: %.cc
	$(CXX) $(CFLAGS) -x c++ -c $< -o $@
