from Crypto.PublicKey import RSA
from Crypto.Signature import pkcs1_15
from Crypto.Hash import SHA512

import os
import struct
import sys

BLOCKSIZE = 1024*1024

def readPEM(pem_filename):
	rsaKey = None
	with open(pem_filename,'r') as pFile:
		rsaKey = RSA.importKey(pFile.read())
	return rsaKey

def fileSHA512(fname):
	iFSize = os.path.getsize(fname)
	rHash = SHA512.new()

	with open(fname, 'rb') as iFile:
		readsize = 0
		while readsize < iFSize:
			rData = iFile.read(BLOCKSIZE)
			rHash.update(rData)
			readsize += len(rData)

	return rHash


def createSignature(iFname, oFname, privKey):
	rHash = fileSHA512(iFname)
	with open(oFname, 'wb') as oFile:
		oFile.write(pkcs1_15.new(privKey).sign(rHash))


def verifySignature(iFname, sFname, pubKey):
	rHash = fileSHA512(iFname)
	with open(sFname,'rb') as sFile:
		signature = sFile.read()

	isSuccess = False
	try:
		pkcs1_15.new(pubKey).verify(rHash, signature)
		print("Signature match")
		isSuccess = True
	except Exception as e:
		print(e)
		print("Signature does not match")

	return isSuccess


def printUsageAndExit():
	print("Usage 1: python {} SIGN   <private key pem filename> <input filename>".format(sys.argv[0]))
	print("Usage 2: python {} VERIFY <public key pem filename>  <input filename> <signature filename>".format(sys.argv[0]))
	sys.exit(1)


if __name__ == '__main__':
	if len(sys.argv) == 4 and sys.argv[1].upper() == 'SIGN':
		privKey = readPEM(sys.argv[2])
		if not privKey.has_private():
			print("{} is not a private key! Use private key".format(sys.argv[2]))
			sys.exit(1)
		iFname = sys.argv[3]
		oFname = iFname + '_signature'
		createSignature(iFname, oFname, privKey)

	elif len(sys.argv) == 5 and sys.argv[1].upper() == 'VERIFY':
		pubKey = readPEM(sys.argv[2])
		if pubKey.has_private():
			print("{} is not a public key! Use public key".format(sys.argv[2]))
			sys.exit(1)
		iFname = sys.argv[3]
		sFname = sys.argv[4]
		verifySignature(iFname, sFname, pubKey)
	else:
		printUsageAndExit()