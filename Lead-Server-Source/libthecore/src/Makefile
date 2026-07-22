CXX = clang++
include ../../platform.mk
BIN_DIR = ../lib
BIN = $(BIN_DIR)/libthecore.a
INCLUDE = ../include

#CFLAGS = -m64 -Wall -O2 -pipe -g -I$(INCLUDE) -Wno-invalid-source-encoding
CFLAGS = -m64 -g -O1 -pipe -frtti -fexceptions -pthread -D_THREAD_SAFE -fno-stack-protector -fno-strict-aliasing -I$(INCLUDE) $(PLATFORM_CFLAGS) $(PLATFORM_CXXFLAGS)

C_SRCS = $(wildcard *.c)
CPP_SRCS = $(wildcard *.cpp)
OBJFILES = $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)

default: $(BIN)

$(BIN): $(OBJFILES)
	mkdir -p $(BIN_DIR)
	ar cru $(BIN) $(OBJFILES)
	ranlib $(BIN)

clean:
	rm -f *.o $(BIN)

%.o: %.c
	$(CXX) $(CFLAGS) -x c++ -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
