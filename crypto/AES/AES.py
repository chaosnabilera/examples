from Crypto.Cipher import AES

import hashlib
import os
import sys
import random

def doAES(iFname, key, mode, oFname):
	key = hashlib.sha256(key.encode('utf-8')).digest()
	iv = bytearray(random.getrandbits(8) for i in range(16))

	ctx = AES.new(key, AES.MODE_CBC, iv)

	if os.path.getsize(iFname) % 16 != 0:
		print("Input file size must be a multiple of 16")
		sys.exit(1)

	with open(iFname, 'rb') as iFile:
		iData = iFile.read()

		with open(oFname, 'wb') as oFile:
			if mode == 'enc':
				# Write iv (which makes encrypted data 16 bytes more than original)
				oFile.write(ctx.encrypt(bytearray(random.getrandbits(8) for i in range(16))))
				# encrypt data
				oFile.write(ctx.encrypt(iData))
			else:
				# consume iv
				ctx.decrypt(iData[:16])
				# decrypt data
				oFile.write(ctx.decrypt(iData[16:]))

def printUsageAndExit():
	print("Usage: python {} <input filename> [enc|dec] <key>".format(sys.argv[0]))
	print("Note:  Input file size must be a multiple of 16 (AES block size)")
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

	doAES(iFname, key, mode, oFname)