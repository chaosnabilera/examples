from Crypto.Cipher import DES3

import hashlib
import os
import sys
import random

def do3DES(iFname, key, mode, oFname):
	keySHA256 = hashlib.sha256(key.encode('utf-8')).digest()
	key = keySHA256[:24]
	iv = bytearray(random.getrandbits(8) for i in range(8))

	ctx = DES3.new(key, DES3.MODE_CBC, iv)

	if os.path.getsize(iFname) % 8 != 0:
		print("Input file size must be a multiple of 8")
		sys.exit(1)

	with open(iFname, 'rb') as iFile:
		iData = iFile.read()

		with open(oFname, 'wb') as oFile:
			if mode == 'enc':
				# Write iv (which makes encrypted data 8 bytes more than original)
				oFile.write(ctx.encrypt(bytearray(random.getrandbits(8) for i in range(8))))
				# encrypt data
				oFile.write(ctx.encrypt(iData))
			else:
				# consume iv
				ctx.decrypt(iData[:8])
				# decrypt data
				oFile.write(ctx.decrypt(iData[8:]))

def printUsageAndExit():
	print("Usage: python {} <input filename> [enc|dec] <key>".format(sys.argv[0]))
	print("Note:  Input file size must be a multiple of 8 (DES block size)")
	sys.exit(1)

if __name__ == '__main__':
	if len(sys.argv) != 4:
		printUsageAndExit()

	iFname = sys.argv[1]
	mode = sys.argv[2].lower()
	if mode not in ['enc','dec']:
		printUsageAndExit()
	key = sys.argv[3]
	oFname = iFname + '_{}'.format(mode)

	do3DES(iFname, key, mode, oFname)