from socket import socket, AF_UNIX, SOCK_STREAM
import json
import os
import struct


SOCKET_PATH = "/tmp/example_socket"


def receive_and_send(conn: socket):
    # PAYLOAD_LENGTH = 4 #bytes
    data = conn.recv(4)
    payload_size = struct.unpack("=I", data)[0]
    print(f"Received: {payload_size}")

    data = conn.recv(payload_size)
    msg = data.decode("utf-8")
    print(f"Received: {msg}")

    try:
        received_json = json.loads(msg)
        response = create_response(received_json)
    except Exception as e:
        response = create_error("error", str(e))

    response_bytes = json.dumps(response).encode("utf-8")
    print("Sent:", response)
    _ = conn.send(struct.pack("=I", len(response_bytes)))
    _ = conn.send(response_bytes)


def create_response(request: dict[str, str]) -> dict[str, str]:
    response = None
    match request:
        case {"type": "create_session", "username": "error"}:
            response = create_error("error", "random error")
        case {"type": "create_session", "username": "success"}:
            response = {"type": "success"}
        case {"type": "create_session"}:
            response = create_auth_response("secret", "mocking:")
        case {"type": "post_auth_message_response", "response": "bad_password"}:
            response = create_error("auth_error", "Incorrect password")
        case {"type": "post_auth_message_response"}:
            response = create_success()
        case {"type": "start_session"}:
            response = create_success()
        case _:
            response = create_error("error", "Invalid request")

    return response


def create_success():
    return {"type": "success"}


def create_error(error_type: str, description: str):
    return {"type": "error", "error_type": error_type, "description": description}


def create_auth_response(type: str, message: str):
    return {"type": "auth_message", "auth_message_type": type, "auth_message": message}


if os.path.exists(SOCKET_PATH):
    os.remove(SOCKET_PATH)

server = socket(AF_UNIX, SOCK_STREAM)
server.bind(SOCKET_PATH)
server.listen(1)

print(f"Listening on {SOCKET_PATH}")

while True:
    try:
        conn, _ = server.accept()
        print("Connected")

        receive_and_send(conn)

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
