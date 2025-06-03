import socket
import pickle

def recv_all(sock, size):
    """Receive exactly `size` bytes"""
    data = b''
    while len(data) < size:
        more = sock.recv(size - len(data))
        if not more:
            raise EOFError("Connection closed unexpectedly.")
        data += more
    return data

# Setup
HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 5005

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.bind((HOST, PORT))
    server.listen(1)
    print(f"[Receiver] Waiting for connection on port {PORT}...")

    conn, addr = server.accept()
    print(f"[Receiver] Connected to {addr}")

    try:
        while True:
            # Get 4-byte frame length
            length_bytes = recv_all(conn, 4)
            frame_size = int.from_bytes(length_bytes, 'big')

            # Receive frame data
            frame_data = recv_all(conn, frame_size)
            frame = pickle.loads(frame_data)

            # Display
            print(f"[Frame] shape: {frame.shape}")
            print(f"[Frame] RX0 Chirp0: {frame[0, :, 0]}\n")
    except Exception as e:
        print(f"[Receiver] Error: {e}")
    finally:
        conn.close()
        print("[Receiver] Disconnected.")
