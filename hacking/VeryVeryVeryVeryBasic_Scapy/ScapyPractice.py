import scapy.all

protocols = {1:'ICMP', 6:'TCP', 17:'UDP'}

def showPacket(packet):
	src_ip = packet[0][1].src
	dst_ip = packet[0][1].dst
	proto = packet[0][1].proto

	if proto in protocols:
		#print('Protocol: {}: {} -> {}'.format(protocols[proto], src_ip, dst_ip))
		#if proto == 1:
		#	print('Type:[{}], Code:[{}]'.format(packet[0][2].type, packet[0][2].code))
		if proto == 6:
			print('Payload: {}'.format(packet[scapy.all.TCP].payload))

if __name__ == '__main__':
	#filter = 'ip'
	filter = 'tcp port 80'
	scapy.all.sniff(filter=filter, prn=showPacket, count=0)