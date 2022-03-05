import win32gui, win32api, win32ui, win32con

def getScreenshot():
	# 1. Get handle to the desktop and its size
	hwnd = win32gui.GetDesktopWindow()

	# When there are only 1 screen...
	#
	# left,top,right,bottom = win32gui.GetWindowRect(hwnd)
	# height = bottom - top
	# width = right - left

	width = win32api.GetSystemMetrics(win32con.SM_CXVIRTUALSCREEN)
	height = win32api.GetSystemMetrics(win32con.SM_CYVIRTUALSCREEN)
	left = win32api.GetSystemMetrics(win32con.SM_XVIRTUALSCREEN)
	top = win32api.GetSystemMetrics(win32con.SM_YVIRTUALSCREEN)

	# 2. Get device context for desktop using handle from 1
	dcHandle = win32gui.GetWindowDC(hwnd)        # Handle to a device context for the specified window
	dcObj = win32ui.CreateDCFromHandle(dcHandle) # Creates a DC object from an integer handle.

	# 3. Create new device context that is compatible with device context of desktop
	memDC = dcObj.CreateCompatibleDC() 			 # Creates (memory) device context that is compatible with the given argument

	# 4. Create bitmap object
	screenshot = win32ui.CreateBitmap()
	screenshot.CreateCompatibleBitmap(dcObj, width, height)

	# 5. Select bitmap object for memory device context for drawing
	memDC.SelectObject(screenshot)	#  selects an object into the specified device context

	# 6. Copy color data from dcObj to memDC
	memDC.BitBlt((0,0), (width, height), dcObj, (left, top), win32con.SRCCOPY)
	screenshot.SaveBitmapFile(memDC, 'screenshot.bmp')

	# 7. Free resources
	memDC.DeleteDC() # DC from CrateCompatibleDC must be released by DeleteDC
	win32gui.DeleteObject(screenshot.GetHandle())

if __name__ == '__main__':
	getScreenshot()