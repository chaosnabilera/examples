import ctypes
import ctypes.wintypes # consult <python dir>\Lib\ctypes\wintypes.py for details

#
# We do this because default ctypes.windll.kernel32 won't work well because it assumes return integer all the time..
# Need to set prototype for each function properly or else "access violation" occurs for arguments to functions.. 
#

class CTypesKernel32:
    __slots__ = [
        '_dll',
        '_FindClose',
        '_FindFirstFileW',
        '_FindNextFileW',
        '_GetLastError',
    ]

    def __init__(self):
        self._dll = ctypes.WinDLL("kernel32.dll", use_last_error = True)
        self._FindClose = None
        self._FindFirstFileW = None
        self._FindNextFileW = None
        self._GetLastError = None

    @property
    def FindClose(self):
        if self._FindClose == None:
            prototype = ctypes.WINFUNCTYPE(
                ctypes.wintypes.BOOL,
                ctypes.wintypes.HANDLE,
                use_last_error = True
            )
            self._FindClose = prototype(("FindClose", self._dll))

        return self._FindClose

    @property
    def FindFirstFileW(self):
        if self._FindFirstFileW == None:
            prototype = ctypes.WINFUNCTYPE(
                ctypes.wintypes.HANDLE,
                ctypes.wintypes.LPCWSTR,
                ctypes.wintypes.LPWIN32_FIND_DATAW, # type : ctypes._Pointer(WIN32_FIND_DATAW)
                use_last_error = True
            )
            self._FindFirstFileW = prototype(("FindFirstFileW", self._dll))

        return self._FindFirstFileW

    @property
    def FindNextFileW(self):
        if self._FindNextFileW == None:
            prototype = ctypes.WINFUNCTYPE(
                ctypes.wintypes.BOOL,
                ctypes.wintypes.HANDLE,
                ctypes.wintypes.LPWIN32_FIND_DATAW,
                use_last_error = True
            )
            self._FindNextFileW = prototype(("FindNextFileW", self._dll))

        return self._FindNextFileW

    @property
    def GetLastError(self):
        if self._GetLastError == None:
            prototype = ctypes.WINFUNCTYPE(
                ctypes.wintypes.DWORD,
                use_last_error = True
            )
            self._GetLastError = prototype(("GetLastError", self._dll))

        return self._GetLastError
