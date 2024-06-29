import socket

# 定义服务器地址和端口
SERVER_HOST = '0.0.0.0'  # 监听所有网络接口
SERVER_PORT = 8080

def start_tcp_server():
    # 创建TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        # 绑定地址和端口
        server_socket.bind((SERVER_HOST, SERVER_PORT))
        # 开始监听连接
        server_socket.listen()
        print(f"[*] TCP 服务器启动，监听地址 {SERVER_HOST}:{SERVER_PORT}")

        while True:
            # 等待客户端连接
            client_socket, client_address = server_socket.accept()
            print(f"[*] 接受来自 {client_address[0]}:{client_address[1]} 的连接")

            # 接收客户端发送的数据
            data = client_socket.recv(1024)
            if data:
                # 打印接收到的数据
                print(f"[+] 收到来自客户端的数据: {data.decode()}")

                # 在这里可以处理接收到的数据，例如存储到文件或者数据库

            # 关闭客户端连接
            client_socket.close()

if __name__ == "__main__":
    start_tcp_server()
