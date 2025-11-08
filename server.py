import socket
import json
import os
import struct

SOCKET_PATH = "/tmp/example_socket"
os.environ["GREETD_SOCK"] = SOCKET_PATH

if os.path.exists(SOCKET_PATH):
    os.remove(SOCKET_PATH)

server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
server.bind(SOCKET_PATH)
server.listen(1)

print(f"Listening on {SOCKET_PATH}")

while True:
    try:
        conn, _ = server.accept()
        print("Connected")

        # PAYLOAD_LENGTH = 4 #bytes
        data = conn.recv(4)
        payload_size = struct.unpack("=I", data)[0]
        print(f"Received: {payload_size}")

        data = conn.recv(payload_size)
        msg = data.decode("utf-8")
        print(f"Received: {msg}")

        try:
            received_json = json.loads(msg)
            username = received_json["username"]
            if username == "error":
                response = {
                    "type": "error",
                    "error_type": "error",
                    "description": "random error",
                }
            else:
                response = {"type": "success"}
        except Exception as e:
            print(f"Error: {e}")
            response = {"type": "error"}

        response_bytes = json.dumps(response).encode("utf-8")
        print("Sent:", response)
        _ = conn.send(struct.pack("=I", len(response_bytes)))
        _ = conn.send(response_bytes)

        conn.close()
    except KeyboardInterrupt:
        print("KeyboardInterrupt")
        break
    except Exception as e:
        print(f"Error: {e}")
        break
server.close()
os.remove(SOCKET_PATH)

print("Closed")
