# -*- coding: utf-8 -*-

# Python 3 script

import os
import socket
import struct
import sys

def connectTo(host, port):
	conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	serverAddress = (host, port)
	conn.connect(serverAddress)
	return conn

def transferFile(sock, filePath):
	with open(filePath, 'rb') as iFile:
		fileName = os.path.split(filePath)[1]
		fileSize = os.stat(filePath).st_size

		print("Sending {}({} bytes)".format(fileName, fileSize))

		#Send fileName length as unsigned 64bit integer
		sock.send(struct.pack('Q',len(fileName)))
		sock.send(fileName)
		#Send fileSize as unsigned 64bit integer
		sock.send(struct.pack('Q',fileSize))

		written = 0
		while(written < fileSize):
			# seek written th byte from beginning and read from there
			iFile.seek(written, 0)
			sData = iFile.read(1024*1024)
			written += sock.send(sData)
			print("{}/{}".format(written,fileSize))


if __name__ == '__main__':
	if len(sys.argv) != 4:
		print("Usage: {} <server ip> <server port> <file path>".format(sys.argv[0]))
		sys.exit(1)

	serverIp = sys.argv[1]
	serverPort = int(sys.argv[2])
	filePath = sys.argv[3]

	if not os.path.isfile(filePath):
		print("{} is not a file".format(filePath))
		sys.exit(1)

	sock = connectTo(serverIp, serverPort)
	transferFile(sock, filePath)