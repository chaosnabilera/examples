import ctypes
import ctypes.wintypes # consult <python dir>\Lib\ctypes\wintypes.py for details

import sys

import ctypes_util.constant
import ctypes_util.kernel32

def print_usage():
    print(f'Usage: {sys.argv[0]} <directory to list>')

def ctypes_list_directory(dir):
    result = []
    fdata = ctypes.wintypes.WIN32_FIND_DATAW()

    # sanitize argument
    while len(dir) > 0 and dir[-1] == '\\':
        dir = dir[:-1]
    if len(dir) == 0:
        dir = '.'
    dir += '\\*'

    kernel32_dll = ctypes_util.kernel32.CTypesKernel32()

    FindFirstFileW = kernel32_dll.FindFirstFileW
    FindNextFileW = kernel32_dll.FindNextFileW
    GetLastError = kernel32_dll.GetLastError
    FindClose = kernel32_dll.FindClose

    hfind = FindFirstFileW(ctypes.c_wchar_p(dir), ctypes.byref(fdata))

    if hfind == ctypes_util.constant.INVALID_HANDLE_VALUE:
        print(f'FindFirstFileW {dir} failed')
        return False
    
    while True:
        result.append(fdata.cFileName)
        if FindNextFileW(hfind, ctypes.byref(fdata)) == 0:
            # print(f'GetLastError : {hex(GetLastError())}')
            break
    
    FindClose(hfind)

    return result

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print_usage()
        sys.exit(0)
    
    result = ctypes_list_directory(sys.argv[1])

    if result:
        for r in result:
            print(r)