#include "libinclude.h"

#include "dprintf.hpp"

void libfunc1(){
    OutputDebugStringA("libfunc1");
    dprintf("debug libfunc1");
}