import os
import sys

def createRandomFile(size, oFname):
	with open(sys.argv[2], 'wb') as oFile:
		while size > 0:
			writeSize = 1024*1024 if (size > 1024*1024) else size
			#oFile.write(bytearray(random.getrandbits(8) for r in range(writeSize)))
			oFile.write(os.urandom(writeSize)) # This is much faster
			size -= writeSize

if len(sys.argv) != 3:
	print('Usage: python {} <size> <output filename>'.format(sys.argv[0]))
	sys.exit(1)

createRandomFile(int(sys.argv[1]), sys.argv[2])