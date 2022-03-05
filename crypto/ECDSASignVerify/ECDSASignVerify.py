from Crypto.PublicKey import ECC
from Crypto.Signature import DSS
from Crypto.Hash import SHA512

import os
import sys
import random

BLOCKSIZE = 1024*1024

def createECDSAPEM(pubk_filename, privk_filename):
	privKey = ECC.generate(curve='P-521') # P-256, P-384, P-521 are supported
	pubKey = privKey.public_key()

	with open(pubk_filename, 'w') as pubKF:
		pubKF.write(pubKey.export_key(format='PEM'))
	with open(privk_filename, 'w') as privKF:
		privKF.write(privKey.export_key(format='PEM'))

def readECDSAPEM(iFname):
	key = None
	with open(iFname, 'r') as iFile:
		key = ECC.import_key(iFile.read())
	return key

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
		signer = DSS.new(privKey, 'fips-186-3')
		oFile.write(signer.sign(rHash))

def verifySignature(iFname, sFname, pubKey):
	rHash = fileSHA512(iFname)
	verifier = DSS.new(pubKey, 'fips-186-3')

	isSuccess = False
	with open(sFname, 'rb') as sFile:
		signature = sFile.read()
	try:
		verifier.verify(rHash, signature)
		print("Signature match")
		isSuccess = True
	except Exception as e:
		print(e)
		print("Signature does not match")

	return isSuccess

def printUsageAndExit():
	print("Usage 1: python {} CREATEKEY <key prefix>".format(sys.argv[0]))
	print("Usage 2: python {} SIGN   <private key pem filename> <input filename>".format(sys.argv[0]))
	print("Usage 3: python {} VERIFY <public key pem filename>  <input filename> <signature filename>".format(sys.argv[0]))
	sys.exit(1)

if __name__ == '__main__':
	if len(sys.argv) == 3 and sys.argv[1].upper() == 'CREATEKEY':
		pubk_filename  = sys.argv[2] + '_pub.pem'
		privk_filename = sys.argv[2] + '_priv.pem'
		createECDSAPEM(pubk_filename, privk_filename)
	elif len(sys.argv) == 4 and sys.argv[1].upper() == 'SIGN':
		privKey = readECDSAPEM(sys.argv[2])
		iFname = sys.argv[3]
		oFname = iFname + '_signature'
		createSignature(iFname, oFname, privKey)

	elif len(sys.argv) == 5 and sys.argv[1].upper() == 'VERIFY':
		pubKey = readECDSAPEM(sys.argv[2])
		iFname = sys.argv[3]
		sFname = sys.argv[4]
		verifySignature(iFname, sFname, pubKey)
	else:
		printUsageAndExit()