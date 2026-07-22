CXX      = clang++
include ../platform.mk
BIN      = ./libpoly.a

#CFLAGS   = -m64 -Wall -O2 -pipe -fno-exceptions -fno-rtti
CFLAGS = -m64 -g -O1 -pipe -frtti -fexceptions -pthread -D_THREAD_SAFE -fno-stack-protector -fno-strict-aliasing $(PLATFORM_CFLAGS) $(PLATFORM_CXXFLAGS)

SRCS     = main.cc Base.cc Poly.cc SymTable.cc Symbol.cc
OBJFILES = $(SRCS:.cc=.o)

default: $(BIN)

$(BIN): $(OBJFILES)
	ar cru $(BIN) $(OBJFILES)
	ranlib $(BIN)

clean:
	rm -f *.o $(BIN)

%.o: %.cc
	$(CXX) $(CFLAGS) -c $< -o $@
