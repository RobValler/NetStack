# uds_test_server.py
import socket
import os
import threading

SOCKET_PATH = "/tmp/uds_test.sock"

stop_flag = False


def wait_for_keypress():
    global stop_flag
    input("\nPress ENTER to stop the server...\n")
    stop_flag = True


# Start keypress watcher in background
threading.Thread(target=wait_for_keypress, daemon=True).start()

# Cleanup old socket file
if os.path.exists(SOCKET_PATH):
    os.remove(SOCKET_PATH)

server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
server.bind(SOCKET_PATH)
server.listen(5)

print("UDS test server running...")
print("Socket:", SOCKET_PATH)

try:
    while not stop_flag:
        server.settimeout(1.0)  # allows periodic stop check

        try:
            conn, _ = server.accept()
        except socket.timeout:
            continue

        print("\nClient connected")

        try:
            while True:
                data = conn.recv(4096)
                if not data:
                    break

                msg = data.decode()
                print("Received:", msg)

                # Simple response for your C++ test
                response = "Here is your shrubbery!"
                conn.sendall(response.encode())
                print("Sent:", response)

        except Exception as e:
            print("Connection error:", e)

        finally:
            conn.close()
            print("Client disconnected")

finally:
    print("\nShutting down server...")
    server.close()

    if os.path.exists(SOCKET_PATH):
        os.remove(SOCKET_PATH)

    print("Cleanup done.")
