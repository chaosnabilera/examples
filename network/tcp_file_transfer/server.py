# -*- coding: utf-8 -*-

# Python 3 script

import socket
import struct
import sys

def prepareServerSocket(ip, port, listenQueueSize = 1):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	serverAddr = (ip, port)
	sock.bind(serverAddr)
	sock.listen(listenQueueSize)
	return sock

def downloadFile(client):
	fileNameLen = client.recv(8)
	fileNameLen = struct.unpack("Q",fileNameLen)[0]
	fileName = client.recv(fileNameLen)
	
	fileSize = client.recv(8)
	fileSize = struct.unpack("Q",fileSize)[0]

	print("Downloading {} ({})bytes".format(fileName, fileSize))

	written = 0
	with open(fileName, "wb") as oFile:
		while written < fileSize:
			bytesToRecv = 1024*1024 if fileSize-written > 1024*1024 else fileSize-written
			iData = client.recv(bytesToRecv)
			oFile.write(iData)
			written += len(iData)
			print("{}/{}".format(written,fileSize))
	print("Download finished")

if __name__ == '__main__':
	if len(sys.argv) != 3:
		print("Usage: {} <ip to bind> <port to bind>".format(sys.argv[0]))
		sys.exit(1)

	serverIp = sys.argv[1]
	serverPort = int(sys.argv[2])

	serverSock = prepareServerSocket(serverIp, serverPort)

	while True:
		(client, addr) = serverSock.accept()
		downloadFile(client)