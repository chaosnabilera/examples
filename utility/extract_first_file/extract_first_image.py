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
	if name[-4:].lower() in ['.jpg','.bmp','.png','.gif','.tif']:
		return True
	if name[-5:].lower() in ['.avif','.webp','.jpeg','.tiff','.jfif']:
		return True
	return False

def getFirstImageFromZip(target_dir):
	target_dir = os.path.abspath(target_dir)

	ziplist = []
	nonzipset = set()

	for rootdir, dirs, files in os.walk(target_dir):
		for fn in files:
			if fn[-4:].lower() == '.zip':
				ziplist.append(os.path.join(rootdir,fn))
			else:
				nonzipset.add(os.path.join(rootdir,fn))

	ziplist.sort()

	for i,zfn in enumerate(ziplist):
		if i % 1000 == 0:
			print(f'Processing: {i}/{len(ziplist)}')

		has_image = False
		zf = os.path.splitext(zfn)[0]
		for ext in ['.jpg','.bmp','.png','.avif','.webp','.jpeg','.gif','.tif','.tiff','.jfif']:
			if zf + ext in nonzipset or zf + ext.upper() in nonzipset:
				# print('Already have image file: ' + zf+ext)
				has_image = True
				break
		
		if has_image:
			continue

		with ZipFile(zfn,'r') as curZip:
			nl = filter(lambda n: isImageFile(n), curZip.namelist())
			nl = list(nl)

			if len(nl) == 0:
				print('No image file in zip: ' + zfn)
				continue

			print(f'Processing {zfn}')

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

	for root, dirs, files in os.walk(sys.argv[1]):
		for dn in dirs:
			getFirstImageFromZip(os.path.join(root,dn))
	getFirstImageFromZip(sys.argv[1])