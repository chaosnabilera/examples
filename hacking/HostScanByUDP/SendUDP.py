import os
import socket
import sys
import time

from netaddr import IPNetwork, IPAddress

def sendMessageToAllSubnet(subnet, port, msg):
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	for ip in IPNetwork(subnet):
		print('Sending message to {}'.format(ip))
		sock.sendto(msg.encode('utf-8'), (str(ip), port))

if __name__ == '__main__':
	host = sys.argv[1]
	subnet = host + '/24'
	msg = 'KNOCK!KNOCK!'
	sendMessageToAllSubnet(subnet, 8080, msg)