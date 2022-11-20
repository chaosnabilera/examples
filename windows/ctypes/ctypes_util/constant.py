import ctypes

MAX_PATH = 260
INVALID_HANDLE_VALUE = 0xFFFFFFFFFFFFFFFF if ctypes.sizeof(ctypes.c_void_p) == 8 else 0xFFFFFFFF