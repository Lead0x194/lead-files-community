%module cipher

%{
#include "cipher.h"
%}

%include cpointer.i
%pointer_functions(int, intp);

%include "cipher.h"
