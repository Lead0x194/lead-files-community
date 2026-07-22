CXX = clang++
include ../platform.mk
BIN = ./libsql.a

EXTERN_DIR = ../../Lead-Extern
INCDIR = -I$(EXTERN_DIR)/include -I../libthecore/include
#CFLAGS = -m64 -Wall -O2 -pipe -D_THREAD_SAFE -fno-exceptions $(INCDIR)
CFLAGS = -m64 -g -O1 -pipe -frtti -fexceptions -pthread -D_THREAD_SAFE -fno-stack-protector -fno-strict-aliasing $(INCDIR) $(PLATFORM_CFLAGS) $(PLATFORM_CXXFLAGS)

SRCS = $(wildcard *.cpp)
OBJFILES = $(SRCS:.cpp=.o)

default: $(BIN)

$(BIN): $(OBJFILES)
	ar cru $(BIN) $(OBJFILES)
	ranlib $(BIN)

clean:
	rm -f *.o $(BIN)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
