# OpenSSL을 이용한 Self-signed certificate + Private key 생성 one liner (테스트 용으로는 이게 제일 편리할 듯?)

~~~
openssl req -new -newkey rsa:4096 -x509 -days 3650 -nodes -keyout server.key -out server.crt
~~~

### req
- https://www.openssl.org/docs/man1.0.2/man1/openssl-req.html
- The req command primarily creates and processes certificate requests in PKCS#10 format. It can additionally create self signed certificates for use as root CAs for example.

### -new
- this option generates a new certificate request

### -newkey arg
- this option creates a new certificate request and a new private key

### -x509
- this option outputs **a self signed certificate** instead of a certificate request

### -days n
- when the -x509 option is being used this specifies the number of days to certify the certificate for. The default is 30 days.

### -nodes
- if this option is specified then if a private key is created it will **not be** encrypted.

### -keyout filename
- this gives the filename to write the newly created private key to

### -out filename
- This specifies the output filename to write to or standard output by default

# 다단계 Private key - public key - certificate sigining request - self-signed certificate 생성

- https://documentation.meraki.com/SM/Other_Topics/Creating_a_Public%2F%2FPrivate_Certificate_Pair

## 1. Generate the private.pem key
~~~
openssl genrsa -out private.pem 2048
~~~

### genrsa
- https://www.openssl.org/docs/man1.1.1/man1/openssl-genrsa.html
- The genrsa command generates an RSA private key.

### Private key 암호화 option
- -aes128, -aes192, -aes256, -aria128, -aria192, -aria256, -camellia128, -camellia192, -camellia256, -des, -des3, -idea

## 2. Generate the public.pem key
~~~
openssl rsa -in private.pem -outform PEM -pubout -out public.pem
~~~

## 3. Create a CSR (Certificate Signing Request) certificate.csr
~~~
openssl req -new -key private.pem -out certificate.csr
~~~

- 위의 one liner의 short ver. 앞서 생성한 private key를 이용해 csr을 만든다
- 위의 것은 self signed certificate을 바로 만드는데 여기서는 먼저 Certificate Signing Request라는걸 만듦
- 이 CSR이라는걸 CA (Certificate Authority)에게 주면 그 CA가 CSR을 이용해 자기가 sign한 certificate를 준다

## 4. Create a self-signed certificate.crt
~~~
openssl x509 -req -days 3650 -in certificate.csr -signkey private.pem -out certificate.crt
~~~

### x509
- https://www.openssl.org/docs/man1.1.1/man1/x509.html
- openssl-x509, x509 - Certificate display and signing utility
- The x509 command is a multi purpose certificate utility. It can be used to display certificate information, convert certificates to various forms, sign certificate requests like a "mini CA" or edit certificate trust settings

### -req
- By default a certificate is expected on input. With this option a certificate request is expected instead.

### -days arg
- Specifies the number of days to make a certificate valid for. The default is 30 days. Cannot be used with the -preserve_dates option.

### -in filename
- This specifies the input filename to read a certificate from or standard input if this option is not specified.

### -signkey arg
- This option causes the input file to be self signed using the supplied private key or engine

### -CA filename
- Specifies the CA certificate to be used for signing. When this option is present x509 behaves like a "mini CA". The input file is signed by this CA using this option: that is its issuer name is set to the subject name of the CA and it is digitally signed using the CAs private key.
- This option is normally combined with the -req option. Without the -req option the input is a certificate which must be self signed.

### -CAkey filename
- Sets the CA private key to sign a certificate with. If this option is not specified then it is assumed that the CA private key is present in the CA certificate file.

### -CAserial filename
- Sets the CA serial number file to use.
- When the -CA option is used to sign a certificate it uses a serial number specified in a file. This file consists of one line containing an even number of hex digits with the serial number to use. After each use the serial number is incremented and written out to the file again.
- The default filename consists of the CA certificate file base name with ".srl" appended. For example if the CA certificate file is called "mycacert.pem" it expects to find a serial number file called "mycacert.srl".

# SSL/TLS client certificate verification with Python v3.4+ SSLContext

- https://www.electricmonk.nl/log/2018/06/02/ssl-tls-client-certificate-verification-with-python-v3-4-sslcontext/
- Server와 Client가 서로서로 상호 verify 할 수 있게 하는 방법
- Client도 Client Certificate를 제공한다

- 그런데 Certificate 생성할때 그냥 엔터엔터 하면 저렇게 만들고 돌렸을때 hostname match 안된다고 인증서 에러 뜬다
	- 관련자료: https://lovian.tistory.com/entry/python%EC%9C%BC%EB%A1%9C-SSLTLS-%EC%82%AC%EC%9A%A9%EC%8B%9C-%EC%9D%B8%EC%A6%9D%EC%84%9C-%EC%9D%B4%EC%8A%88
	- 해결책 1: python의 context에서 (ssl.SSLContext 나 create_default_context 로 생성되는 그거) context.check_hostname = False를 하면 hostname을 check안함
	- 해결책 2: OpenSSL로 생성할 때 Common Name (e.g. server FQDN or YOUR name) 을 넣는 부분이 있다. 여기에 hostname을 적어넣으면 된다 (e.g. 아래의 example.com)

## 1. Create Server Certificate
~~~
openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout server.key -out server.crt
~~~

## 2. Create Client Certificate
~~~
openssl req -new -newkey rsa:2048 -days 365 -nodes -x509 -keyout client.key -out client.crt
~~~

## 3. Client code
```python
#!/usr/bin/python3

import socket
import ssl

host_addr = '127.0.0.1'
host_port = 8082
server_sni_hostname = 'example.com'
server_cert = 'server.crt'
client_cert = 'client.crt'
client_key = 'client.key'

context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=server_cert)
context.load_cert_chain(certfile=client_cert, keyfile=client_key)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
conn = context.wrap_socket(s, server_side=False, server_hostname=server_sni_hostname)
conn.connect((host_addr, host_port))
print("SSL established. Peer: {}".format(conn.getpeercert()))
print("Sending: 'Hello, world!")
conn.send(b"Hello, world!")
print("Closing connection")
conn.close()
```

## 4. Server Code
```python
#!/usr/bin/python3

import socket
from socket import AF_INET, SOCK_STREAM, SO_REUSEADDR, SOL_SOCKET, SHUT_RDWR
import ssl

listen_addr = '127.0.0.1'
listen_port = 8082
server_cert = 'server.crt'
server_key = 'server.key'
client_certs = 'client.crt'

context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
context.verify_mode = ssl.CERT_REQUIRED
context.load_cert_chain(certfile=server_cert, keyfile=server_key)
context.load_verify_locations(cafile=client_certs)

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
```