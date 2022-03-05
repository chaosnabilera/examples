# Caution
* Tested on Windows VM
* Does **NOT** work unless both side of Windows allows ICMP messages
    * Both needs to turn down Windows Firewall, basically...
* Not exactly a good way for host scanning, according to Wikipedia....

# How to use
* Execute SniffICMP.py 
* Execute SendUDP.py
* Look for result where type = 3, code = 3 
    * means Destination unreachable, Port unreachable
    * which means the host has sent response
    * Actually, message should match too. But will not go further because this method sucks anyway...

# Wikipedia - Port scanning - UDP scanning

UDP scanning is also possible, although there are technical challenges. 
UDP is a connectionless protocol so there is no equivalent to a TCP SYN packet. 
However, if a UDP packet is sent to a port that is not open, 
the system will respond with an ICMP port unreachable message. 
Most UDP port scanners use this scanning method, and use the absence of a response to infer that a port is open. 
However, if a port is blocked by a firewall, this method will falsely report that the port is open. 
If the port unreachable message is blocked, all ports will appear open. 
This method is also affected by ICMP rate limiting.[4]

An alternative approach is to send application-specific UDP packets, hoping to generate an application layer response. 
For example, sending a DNS query to port 53 will result in a response, if a DNS server is present. 
This method is much more reliable at identifying open ports. 
However, it is limited to scanning ports for which an application specific probe packet is available. 
Some tools (e.g., nmap) generally have probes for less than 20 UDP services, 
while some commercial tools have as many as 70. 
In some cases, a service may be listening on the port, but configured not to respond to the particular probe packet.