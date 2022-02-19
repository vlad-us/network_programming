from importlib.metadata import requires
import socket


server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('127.0.0.1', 8088))
server_socket.listen()
while True:
    client_socket, addr = server_socket.accept()
    request = client_socket.recv(1024)
    print(f"Client: {addr}")
    print(f"Request: {request.decode()}")
    client_socket.sendall('response'.encode())
    client_socket.close()
