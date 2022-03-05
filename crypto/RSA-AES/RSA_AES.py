from Crypto.Cipher import AES
from Crypto.Cipher import PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Signature import pkcs1_15

import os
import struct
import sys
import random

RSA_KEYSIZE = 4096
BLOCKSIZE = 1024*1024 # Must be multiple of 16

# Encrypted data structure:
# AES key(RSA_KEYSIZE) - AES iv(16) - Data size(8) - Data

def createPEM(pubk_filename, privk_filename, keySize = RSA_KEYSIZE):
	print("Generating private key ({}) -> {}".format(keySize, privk_filename))
	privateKey = RSA.generate(keySize)
	print("Generating public key -> {}".format(pubk_filename))
	publicKey = privateKey.publickey()

	with open(privk_filename, 'wb') as privKF:
		privKF.write(privateKey.exportKey('PEM'))
	with open(pubk_filename, 'wb') as pubKF:
		pubKF.write(publicKey.exportKey('PEM'))

def readPEM(pem_filename):
	rsaKey = None
	with open(pem_filename,'r') as pFile:
		rsaKey = RSA.importKey(pFile.read())
	return rsaKey

def encrypt_RSA_AES(iFname, oFname, pubKey):
	key = bytearray(random.getrandbits(8) for i in range(32))
	iv = bytearray(random.getrandbits(8) for i in range(16))

	rsaCipher = PKCS1_OAEP.new(pubKey)
	ctx = AES.new(key, AES.MODE_CBC, iv)
	iFSize = os.path.getsize(iFname)

	with open(iFname, 'rb') as iFile:
		with open(oFname, 'wb') as oFile:
			# write AES key
			oFile.write(rsaCipher.encrypt(key))
			# write iv
			oFile.write(ctx.encrypt(bytearray(random.getrandbits(8) for i in range(16))))
			# write data size
			oFile.write(ctx.encrypt(struct.pack("QQ", 0, iFSize)))
			# write data
			written = 0
			while written < iFSize:
				curData = iFile.read(BLOCKSIZE)
				if len(curData) % 16 != 0:
					curData += bytearray(16 - (len(curData) % 16))
				oFile.write(ctx.encrypt(curData))
				written += len(curData)


def decrypt_RSA_AES(iFname, oFname, privKey):
	rsaCipher = PKCS1_OAEP.new(privKey)

	with open(iFname, 'rb') as iFile:
		with open(oFname, 'wb') as oFile:
			# get AES key
			key = rsaCipher.decrypt(iFile.read(RSA_KEYSIZE//8))
			iv = bytearray(random.getrandbits(8) for i in range(16))
			ctx = AES.new(key, AES.MODE_CBC, iv)
			# consume iv
			ctx.decrypt(iFile.read(16))
			# get file size
			dummy, filesize = struct.unpack("QQ", ctx.decrypt(iFile.read(16)))
			# write data
			written = 0
			while written < filesize:
				decData = ctx.decrypt(iFile.read(BLOCKSIZE))
				if len(decData) > (filesize-written):
					decData = decData[:filesize-written]
				oFile.write(decData)
				written += len(decData)


def printUsageAndExit():
	print("Usage 1: python {} CREATEKEY <key filename prefix>".format(sys.argv[0]))
	print("         --> Creates RSA key pair of size {} bits".format(RSA_KEYSIZE))
	print("Usage 2: python {} ENCRYPT <input filename> <public key PEM filename>".format(sys.argv[0]))
	print("         --> Encrypts file using RSA+AES where AES key is encrypted with RSA key")
	print("Usage 3: python {} DECRYPT <input filename> <private key PEM filename>".format(sys.argv[0]))
	print("         --> Decrypts file that was encrypted by Usage 2")
	sys.exit(1)

if __name__ == '__main__':
	if len(sys.argv) == 3 and sys.argv[1].upper() == 'CREATEKEY':
		pubk_filename = sys.argv[2] + '_pub.pem'
		privk_filename = sys.argv[2] + '_priv.pem'
		createPEM(pubk_filename, privk_filename)
	elif len(sys.argv) == 4 and sys.argv[1].upper() == 'ENCRYPT':
		pubKey = readPEM(sys.argv[3])
		if pubKey.has_private():
			print("{} is a private key! Use public key".format(sys.argv[3]))
			sys.exit(1)
		iFname = sys.argv[2]
		oFname = iFname + '_enc'
		encrypt_RSA_AES(iFname, oFname, pubKey)
	elif len(sys.argv) == 4 and sys.argv[1].upper() == 'DECRYPT':
		privKey = readPEM(sys.argv[3])
		if not privKey.has_private():
			print("{} is not a private key! Use private key".format(sys.argv[3]))
			sys.exit(1)
		iFname = sys.argv[2]
		oFname = iFname + '_dec'
		decrypt_RSA_AES(iFname, oFname, privKey)
		pass
	else:
		printUsageAndExit()