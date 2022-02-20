from importlib.metadata import requires
import socket

response = "Ipsum quis id ex laboris laborum tempor elit nisi dolor quis irure. Consectetur nulla eiusmod aliqua fugiat commodo velit laborum officia Lorem dolor ullamco in sunt. Do reprehenderit minim aliquip excepteur ad. Est consequat mollit excepteur ex officia cillum in ullamco Lorem cillum elit occaecat adipisicing fugiat. Officia cupidatat deserunt et dolor in et culpa anim aliquip. Esse laborum deserunt qui pariatur. Commodo nostrud labore non nostrud dolor laboris non est nostrud."

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('127.0.0.1', 8088))
server_socket.listen()
while True:
    client_socket, addr = server_socket.accept()
    request = client_socket.recv(4096)
    print(f"Client: {addr}")
    print(f"Request: {request.decode()}")
    client_socket.sendall(response.encode())
    client_socket.close()
