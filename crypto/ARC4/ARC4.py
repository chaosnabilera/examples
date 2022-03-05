from Crypto.Cipher import ARC4

import hashlib
import os
import sys
import random

def doARC4(iFname, key, mode, ivLen, oFname):
	ctx = ARC4.new(key)

	with open(iFname, 'rb') as iFile:
		iData = iFile.read()

		with open(oFname, 'wb') as oFile:
			if mode == 'enc':
				# Write iv
				iv = bytearray(random.getrandbits(8) for i in range(ivLen))
				oFile.write(ctx.encrypt(iv))
				# encrypt data
				oFile.write(ctx.encrypt(iData))
			else:
				# consume iv
				ctx.decrypt(iData[:ivLen])
				# decrypt data
				oFile.write(ctx.decrypt(iData[ivLen:]))


def printUsageAndExit():
	print("Usage: python {} <input filename> [enc|dec] <key> <iv length>".format(sys.argv[0]))
	print("Note : key length for ARC4 should be 40~2048 bits")
	sys.exit(1)


if __name__ == '__main__':
	if len(sys.argv) != 5:
		printUsageAndExit()

	iFname = sys.argv[1]
	mode = sys.argv[2].lower()
	if mode not in ['enc','dec']:
		printUsageAndExit()
	
	key = sys.argv[3].encode('utf-8')
	if len(key) < (40//8) or len(key) > (2048//8):
		printUsageAndExit()

	ivLen = int(sys.argv[4])

	oFname = iFname + '_{}'.format(mode)

	doARC4(iFname, key, mode, ivLen, oFname)