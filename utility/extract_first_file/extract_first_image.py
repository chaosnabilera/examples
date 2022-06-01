import os
import sys
from zipfile import ZipFile

def winSanitizeName(name):
	for c in '\\/:*?|':
		name = name.replace(c,'_')
	name = name.replace('"',"'")
	name = name.replace('<','(')
	name = name.replace('>',')')
	return name

def isImageFile(name):
	if name[-4:].lower() in ['.jpg','.bmp','.png']:
		return True
	if name[-5:].lower() in ['.avif','.webp','.jpeg']:
		return True
	return False

def getFirstImageFromZip(target_dir):
	target_dir = os.path.abspath(target_dir)
	ziplist = []
	for rootdir, dirs, files in os.walk(target_dir):
		for fn in filter(lambda f:f[-4:].lower() == '.zip', files):
			ziplist.append(os.path.join(rootdir,fn))
	ziplist.sort()

	for zfn in ziplist:
		with ZipFile(zfn,'r') as curZip:
			nl = filter(lambda n: isImageFile(n), curZip.namelist())
			nl = list(nl)
			nl.sort()

			finm = nl[0]
			finmExt = '.' + finm.split('.')[-1]

			zfiFile = curZip.open(finm, 'r')

			oFile = open(zfn[:-4]+finmExt, 'wb')
			oFile.write(zfiFile.read())
			oFile.close()

			zfiFile.close()


def print_usage():
	print("Usage: {} <destination folder>".format(sys.argv[0]))

if __name__ == '__main__':
	if len(sys.argv) != 2:
		print_usage()
		sys.exit(1)

	getFirstImageFromZip(sys.argv[1])