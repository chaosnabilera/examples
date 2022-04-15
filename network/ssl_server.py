import getpass
import os
import socket
import ssl
import sys

def setup_mutually_verified_tls_tcp_server(listen_addr, listen_port, privkey_path, privkey_passwd, cert_path, clnt_cert_path):
	context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
	context.verify_mode = ssl.CERT_REQUIRED
	context.load_cert_chain(certfile=cert_path, keyfile=privkey_path, password=privkey_passwd)
	context.load_verify_locations(cafile=clnt_cert_path)

	bindsocket = socket.socket()
	bindsocket.bind((listen_addr, listen_port))
	bindsocket.listen(5)

	while True:
		print("Waiting for client")
		newsocket, fromaddr = bindsocket.accept()
		print("Client connected: {}:{}".format(fromaddr[0], fromaddr[1]))
		conn = context.wrap_socket(newsocket, server_side=True)
		print("SSL established. Peer: {}".format(conn.getpeercert()))
		buf = b''  # Buffer to hold received client data
		try:
			while True:
				data = conn.recv(4096)
				if data:
					# Client sent us data. Append to buffer
					buf += data
				else:
					# No more data from client. Show buffer and close connection.
					print("Received:", buf)
					break
		finally:
			print("Closing connection")
			conn.shutdown(socket.SHUT_RDWR)
			conn.close()

def print_usage_and_exit():
	print(f'{sys.argv[0]} <listen ip addr> <listen port> [-p] <server private key> <server certificate> <client certificate>')
	sys.exit(0)

if __name__ == '__main__':
	argc = len(sys.argv)
	if argc < 6 or argc > 7 or (argc == 7 and sys.argv[3] != '-p'):
		print_usage_and_exit()

	listen_addr = sys.argv[1]
	listen_port = int(sys.argv[2])

	privkey_path, cert_path, clnt_cert_path = sys.argv[-3:]
	for path in [privkey_path, cert_path, clnt_cert_path]:
		if not os.path.isfile(path):
			print(f'{path} does not exist')
			print_usage_and_exit()
	
	if sys.argv[3] == '-p':
		privkey_passwd = getpass.getpass(prompt=f'Password for {privkey_path}:')
	else:
		privkey_passwd = None

	setup_mutually_verified_tls_tcp_server(listen_addr, listen_port, privkey_path, privkey_passwd, cert_path, clnt_cert_path)