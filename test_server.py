import socket

response = "Eu non adipisicing aliquip dolore cillum sint. Nulla ad elit magna dolor elit aliquip fugiat est. Commodo consequat qui fugiat culpa ex id commodo. Nostrud ea anim duis occaecat incididunt. Amet eu sint minim labore labore."

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('127.0.0.1', 6666))
server_socket.listen()
while True:
    client_socket, addr = server_socket.accept()
    request = client_socket.recv(4096)
    print(f"Client: {addr}")
    print(f"Request: {request.decode()}")
    client_socket.sendall(response.encode())
    client_socket.close()
