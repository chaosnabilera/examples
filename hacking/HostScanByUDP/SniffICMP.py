import os
import socket
import struct
import sys
import time

from collections import namedtuple

IPHeader = namedtuple('IPHeader',
	['version', 'header_length', 'service_type', 'entire_packet_length', 
	'datagram_id', 'flag', 'fragment_offset', 
	'time_to_live', 'protocol', 'header_checksum', 
	'source_ipaddress', 
	'destination_ipadress'])

ICMPHeader = namedtuple('ICMPHeader',['ptype','code','checksum','others'])


def parse_ipheader(data):
	ipheader = struct.unpack('!BBHHHBBH4s4s', data[:20])
	# B
	version = (ipheader[0] & 0xF0) >> 4
	header_length = (ipheader[0] & 0xF) * 4 # IHL(IP header length) represents the number of 4byte words in header
	# B
	service_type = ipheader[1]
	# H
	entire_packet_length = ipheader[2]
	# H
	datagram_id = ipheader[3]
	# H
	flag = (ipheader[4] & 0xE000) >> 13
	fragment_offset = (ipheader[4] & 0x1FF)
	# B
	time_to_live = ipheader[5]
	# B
	protocol = ipheader[6]
	protocol_list = {1:'ICMP', 6:'TCP', 17:'UDP'}
	if protocol in protocol_list.keys():
		protocol = protocol_list[protocol]
	# H
	header_checksum = ipheader[7]
	# 4s
	source_ipaddress = ipheader[8]
	source_ipaddress = socket.inet_ntoa(source_ipaddress)
	# 4s
	destination_ipadress = ipheader[9]
	destination_ipadress = socket.inet_ntoa(destination_ipadress)

	return IPHeader(version, header_length, service_type, entire_packet_length,
		datagram_id, flag, fragment_offset, 
		time_to_live, protocol, header_checksum, 
		source_ipaddress, destination_ipadress)


def parse_icmpheader(data):
	ptype, code, checksum, others = struct.unpack('!BBH4s', data[:8])
	return ICMPHeader(ptype, code, checksum, others)


#
# Note: Recv on raw socket in Windows does not consume data
#       Datagrams are **COPIED** into **ALL** raw sockets that meet the condition
#
# Read https://docs.microsoft.com/en-us/windows/win32/winsock/tcp-ip-raw-sockets-2 for details of Raw sockets in Windows
#
def recvDataFromRawSocket(sock):
	data = ''
	try:
		# Max IP datagram size os 65535
		data = sock.recvfrom(65535)
	except socket.timeout:
		data = ''
	return data[0]


def sniffIP(host):
	# IPPROTO_IP - sniff all IP packets, regardless of protocol (TCP, UDP, ICMP)
	sniffer = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_IP)

	# Need to bind to specific NIC
	if host == None:
		print('Automatically setting host')
		host = socket.gethostbyname(socket.gethostname())
	
	print('binding to {}'.format(host))
	sniffer.bind((host, 0))

	# For IPv4 (address family of AF_INET), an application receives the 
	# IP header at the front of each received datagram regardless of the IP_HDRINCL socket option.
	sniffer.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)

	#
	# https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ee309610(v%3Dvs.85)
	#
	# Turns on promiscuous mode, which enables socket to receive all 
	# IPv4 or IPv6 packets passing through a network interface 
	# a NIC that supports promiscuous mode will capture all IPv4 or IPv6 traffic on the LAN, 
	# including traffic between other computers on the same LAN segment
	sniffer.ioctl(socket.SIO_RCVALL, socket.RCVALL_ON)

	count = 1
	while True:
		data = recvDataFromRawSocket(sniffer)
		ipheader = parse_ipheader(data[:20])
		if ipheader.protocol == 'ICMP':
			icmpheader = parse_icmpheader(data[ipheader.header_length:ipheader.header_length+8])
			print(ipheader.source_ipaddress, ipheader.destination_ipadress, icmpheader)
		count += 1
	"""
	except KeyboardInterrupt:
		if os.name == 'nt':
			sniffer.ioctl(socket.SIO_RCVALL, socket.RCVALL_OFF)
	"""

def printUsageAndExit():
	print("Prints ICMP messages using Raw socket")
	print("Usage 1: python {}".format(sys.argv[0]))
	print("Usage 2: python {} <IPv4 address to bind>".format(sys.argv[0]))
	sys.exit(1)

if __name__ == '__main__':
	host = None

	if len(sys.argv) > 2:
		printUsageAndExit()

	if len(sys.argv) == 2:
		host = sys.argv[1]
		if host == '0.0.0.0':
			print('Need to bind to specific address')
			sys.exit(1)

	sniffIP(host)

	