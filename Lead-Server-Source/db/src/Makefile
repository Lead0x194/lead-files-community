CXX = clang++
include ../../platform.mk
BINDIR = ..
OBJDIR = .obj
TARGET = $(BINDIR)/db

EXTERN_DIR = ../../../Lead-Extern
SHARED_SRC = ../../../Lead-Shared-Source

INCDIR = -I$(EXTERN_DIR)/include \
         -I$(EXTERN_DIR)/include/libpng \
         -I$(EXTERN_DIR)/include/libjpeg-turbo \
         -I$(SHARED_SRC) \
         -I../../libthecore/include \
         -I../../libgame/include \
         -I../../libsql \
         -I../../libpoly \
         -I../../boost

LIBDIR = -L../../libthecore/lib -L../../libsql -L../../libpoly -L../../libgame/lib
LIBDIR += -L/usr/local/lib/mysql/

#CFLAGS = -m64 -g -Wall -O2 -pipe -fno-rtti -fno-exceptions -pthread -D_THREAD_SAFE
CFLAGS = -m64 -g -O1 -pipe -frtti -fexceptions -pthread -D_THREAD_SAFE -fno-stack-protector -fno-strict-aliasing $(PLATFORM_CFLAGS) $(PLATFORM_CXXFLAGS)

ifeq ($(HOST_OS),Linux)
LIBS = ../../libsql/libsql.a \
       ../../libpoly/libpoly.a \
       ../../libgame/lib/libgame.a \
       ../../libthecore/lib/libthecore.a \
       -lmariadb -lssl -lcrypto -lm -lz -pthread
else
LIBS = ../../libsql/libsql.a \
       ../../libpoly/libpoly.a \
       ../../libgame/lib/libgame.a \
       /usr/lib/libssl.a /usr/lib/libcrypto.a \
       ../../libthecore/lib/libthecore.a \
       -lmariadbclient -lm -lz
endif

SRCS = $(filter-out version.cpp, $(wildcard *.cpp))
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

default: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS) -o $(TARGET)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CFLAGS) $(INCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
