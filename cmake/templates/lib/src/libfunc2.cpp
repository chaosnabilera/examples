#include "libinclude.h"

#include "dprintf.hpp"

void libfunc2(){
    OutputDebugStringA("libfunc2");
    dprintf("debug libfunc2");
}