HOST_OS := $(shell uname -s)

# Linux is kept as an explicit project platform flag. FreeBSD continues to use
# the compiler-provided __FreeBSD__ macro, and Visual Studio continues to define
# the existing Windows macros in the project files.
ifeq ($(HOST_OS),Linux)
PLATFORM_CFLAGS := -D__LINUX__
PLATFORM_CXXFLAGS := -std=gnu++17
else
PLATFORM_CFLAGS :=
PLATFORM_CXXFLAGS :=
endif
