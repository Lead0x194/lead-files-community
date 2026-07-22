CXX = clang++
CC = clang
include ../../platform.mk
BINDIR = ..
OBJDIR = .obj
TARGET = $(BINDIR)/game

EXTERN_DIR = ../../../Lead-Extern
SHARED_SRC = ../../../Lead-Shared-Source

INCDIR = -I$(EXTERN_DIR)/include \
         -I$(EXTERN_DIR)/include/libpng \
         -I$(EXTERN_DIR)/include/libjpeg-turbo \
         -I$(EXTERN_DIR)/include/boost \
         -I$(SHARED_SRC) \
         -I../../liblua/include \
         -I../../libthecore/include \
         -I../../libgame/include \
         -I$(EXTERN_DIR)/include/lzo-2.10/include \
         -I/usr/local/include

LIBDIR = -L$(EXTERN_DIR)/lib \
         -L../../libthecore/lib \
         -L../../libpoly \
         -L../../libsql \
         -L../../libgame/lib \
         -L../../liblua/lib \
         -L/usr/local/lib

LIBDIR += -L/usr/local/lib/mysql/

#CFLAGS = -m64 -g -Wall -O2 -pipe -fexceptions -fno-stack-protector -pthread -D_THREAD_SAFE -DNDEBUG
CFLAGS = -m64 -g -O1 -pipe -frtti -fexceptions -pthread -D_THREAD_SAFE -DBOOST_BIND_GLOBAL_PLACEHOLDERS -fno-stack-protector -fno-strict-aliasing $(PLATFORM_CFLAGS) $(PLATFORM_CXXFLAGS)

# RE-ORDERED LIBS: Higher level libs first, base libs last
ifeq ($(HOST_OS),Linux)
LIBS = ../../liblua/lib/liblua.a \
       ../../liblua/lib/liblualib.a \
       ../../libsql/libsql.a \
       ../../libgame/lib/libgame.a \
       ../../libpoly/libpoly.a \
       ../../libthecore/lib/libthecore.a \
       -lmariadb -lcryptopp -lIL -llzo2 -lssl -lcrypto -lz -lm -pthread
else
LIBS = -L/usr/local/lib -llzo2 \
       ../../liblua/lib/liblua.a \
       ../../liblua/lib/liblualib.a \
       ../../libsql/libsql.a \
       ../../libgame/lib/libgame.a \
       ../../libpoly/libpoly.a \
       /usr/lib/libssl.a /usr/lib/libcrypto.a \
       ../../libthecore/lib/libthecore.a \
       -lmariadbclient -lcryptopp -lIL -lz -lm -lmd -pthread
endif

CPP_SRCS = $(wildcard *.cpp)
C_SRCS = $(wildcard *.c)
OBJS = $(CPP_SRCS:%.cpp=$(OBJDIR)/%.o) $(C_SRCS:%.c=$(OBJDIR)/%.o)

default: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS) -o $(TARGET)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CFLAGS) $(INCDIR) -c $< -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
