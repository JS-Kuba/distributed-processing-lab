import socket
import threading
# import argparse

# Server configuration
HOST = '172.19.0.2'
PORT = 12345
BUFFER_SIZE = 1024    # Buffer size for receiving messages

def handle_received_message(client_socket):
    """
    Thread function to handle receiving messages from server
    """
    while True:
        try:
            # Receive message from server
            data = client_socket.recv(BUFFER_SIZE).decode('utf-8')
            if data:
                print(data)
            else:
                # If data is empty, the server has closed the connection
                client_socket.close()
                break
        except Exception as e:
            print(f'Error: {e}')
            client_socket.close()
            break

def start_client():
    """
    Starts the client and connects to the server
    """
    # Create socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((HOST, PORT))
    print(f'Connected to {HOST}:{PORT}')
    # Start a thread to handle receiving messages from server
    receive_thread = threading.Thread(target=handle_received_message, args=(client_socket,))
    receive_thread.start()

    while True:
        try:
            # Send message to server
            message = input()
            print(f"Send message: {message}\n")
            client_socket.sendall(message.encode('utf-8'))
        except KeyboardInterrupt:
            # Close the client socket when user interrupts the program
            client_socket.close()
            break

if __name__ == '__main__':
    start_client()

