CXXSRCS		= cipher.cxx sequence.cxx
TARGET		= cipher
INTERFACE	= cipher.i

INCLUDES	= -I./include
LIBS		= -L../../lib -lcryptopp 

SWIG       = swig
SWIGPYTHON = $(SWIG) -python
PYTHON_SO     = .so

CCSHARED	= -fPIC
CXX			= g++
CFLAGS		= 
CXXSHARED	= g++ -shared
OBJS      = $(SRCS:.c=.o) $(CXXSRCS:.cxx=.o)
ISRCS      = $(IWRAP:.i=.c)
ICXXSRCS   = $(IWRAP:.i=.cxx)
IWRAP      = $(INTERFACE:.i=_wrap.i)
IOBJS      = $(IWRAP:.i=.o)

all: 
	$(SWIGPYTHON) -c++ $(INTERFACE)
	$(CXX) -c $(CCSHARED) $(CFLAGS) $(ICXXSRCS) $(SRCS) $(CXXSRCS) $(INCLUDES) $(PYTHON_INCLUDE) 
	$(CXXSHARED) $(CFLAGS) $(OBJS) $(IOBJS) $(LIBS) $(CPP_DLLIBS) -o _$(TARGET)$(PYTHON_SO) 

clean:
	rm -f *_wrap* *~ .~* mypython *.pyc
	rm -f core 
	rm -f *.o *.so *.so
	rm -f $(TARGET).py 
