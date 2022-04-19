import getpass
import socket
import os
import ssl
import sys

def make_mutually_verified_ssl_connection(srv_ip, srv_port, privkey_path, privkey_passwd, cert_path, srv_cert_path):
	context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=srv_cert_path)
	context.load_cert_chain(certfile=cert_path, keyfile=privkey_path, password=privkey_passwd)
	context.check_hostname = False # otherwise, we need to put Common Name / FQDN
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	conn = context.wrap_socket(s, server_side=False)
	conn.connect((srv_ip, srv_port))

	print("SSL established. Peer: {}".format(conn.getpeercert()))
	print("Sending: 'Hello, world!")
	conn.send(b"Hello, world!")
	print("Closing connection")
	conn.close()


def print_usage_and_exit():
	print(f'Usage : {sys.argv[0]} <server ip> <server port> [-p] <client private key> <client certificate> <server certificate>')
	sys.exit(0)

if __name__ == '__main__':
	argc = len(sys.argv)
	if argc < 6 or argc > 7 or (argc == 7 and sys.argv[3] != '-p'):
		print_usage_and_exit()

	srv_ip = sys.argv[1]
	srv_port = int(sys.argv[2])

	privkey_path, cert_path, srv_cert_path = sys.argv[-3:]
	for path in [privkey_path, cert_path, srv_cert_path]:
		if not os.path.isfile(path):
			print(f'{path} does not exist')
			print_usage_and_exit()

	if sys.argv[3] == '-p':
		privkey_passwd = getpass.getpass(prompt=f'Password for {privkey_path}:')
	else:
		privkey_passwd = None

	make_mutually_verified_ssl_connection(srv_ip, srv_port, privkey_path, privkey_passwd, cert_path, srv_cert_path)
	
