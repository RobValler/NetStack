# uds_test_server.py
import socket
import os
import threading

SOCKET_PATH = "/tmp/uds_test.sock"

stop_flag = False
last_response = "NO DATA"


def wait_for_keypress():
    global stop_flag
    input("\nPress ENTER to stop the server...\n")
    stop_flag = True


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
        server.settimeout(1.0)

        try:
            conn, _ = server.accept()
        except socket.timeout:
            continue

        print("\nClient connected")

        try:
            # Try to receive data (non-blocking style)
            conn.settimeout(0.5)

            try:
                data = conn.recv(4096)
            except socket.timeout:
                data = b""

            if data:
                msg = data.decode()
                print("Received:", msg)

                # Store response for next Receive() call
                last_response = "Here is your shrubbery!"

                # IMPORTANT: Do NOT rely on this response for your test
                # (your C++ client won't read it here)
                conn.sendall(last_response.encode())
                print("Sent (immediate):", last_response)

            else:
                # This is the Receive() connection
                print("Receive() connection detected")

                conn.sendall(last_response.encode())
                print("Sent (deferred):", last_response)

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
