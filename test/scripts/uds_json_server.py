#!/usr/bin/env python3

import json
import os
import signal
import socket
import sys

SOCKET_PATH = "/tmp/uds_test.sock"


def get_device_data():
    return [
        {
            "device_name": "sda1",
            "mount_point": "/mnt/data",
            "other": "primary disk"
        },
        {
            "device_name": "sdb1",
            "mount_point": "/mnt/backup",
            "other": "backup disk"
        },
        {
            "device_name": "nvme0n1p1",
            "mount_point": "/mnt/fast",
            "other": "nvme storage"
        }
    ]


def cleanup():
    if os.path.exists(SOCKET_PATH):
        os.unlink(SOCKET_PATH)


def signal_handler(sig, frame):
    cleanup()
    sys.exit(0)


def main():
    cleanup()

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    server.bind(SOCKET_PATH)
    server.listen(5)

    print(f"Listening on {SOCKET_PATH}")

    while True:
        conn, _ = server.accept()

        try:
            request = conn.recv(4096).decode("utf-8").strip()

            if request == "list":
                response = json.dumps(get_device_data())
            else:
                response = json.dumps({
                    "error": f"Unknown command: {request}"
                })

            conn.sendall(response.encode("utf-8"))

        except Exception as e:
            conn.sendall(
                json.dumps({"error": str(e)}).encode("utf-8")
            )

        finally:
            conn.close()


if __name__ == "__main__":
    main()
