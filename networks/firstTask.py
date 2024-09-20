import argparse
import ipaddress
import socket
import struct
import threading
import time
import uuid

PORT = 9999
INTERVAL = 2
EXPIRY_DURATION = 6

peers = {}
peers_lock = threading.Lock()
my_id = str(uuid.uuid4())


class Config:
    def __init__(self, protocol, multicast_address, interface_name=None):
        self.protocol = protocol
        self.multicast_address = multicast_address
        self.interface_name = interface_name


def parse_args():
    parser = argparse.ArgumentParser(description="Network Discovery Application")
    parser.add_argument("multicast_address", help="Multicast address")
    parser.add_argument("interface_name", nargs="?", help="Interface name (required for IPv6)")
    args = parser.parse_args()

    if ":" in args.multicast_address:
        protocol = "ipv6"
        if not args.interface_name:
            parser.error("Interface name is required for IPv6")
    else:
        protocol = "ipv4"

    return Config(protocol, args.multicast_address, args.interface_name)


def get_interface(config):
    if config.protocol == "ipv6":
        return socket.if_nametoindex(config.interface_name)
    return 0  # For IPv4, we don't need to specify an interface


def send(sock, addr):
    print("Sending presence messages...")
    while True:
        message = f"I'm here, ID: {my_id}".encode()
        sock.sendto(message, addr)
        time.sleep(INTERVAL)


def receive(sock):
    print("Waiting for messages from other copies...")
    while True:
        data, addr = sock.recvfrom(1024)
        message = data.decode().strip()
        if "I'm here" in message and my_id not in message:
            print(f"Received message from: {addr[0]} ({message})")
            update_peer(f"{addr[0]} ({message})")


def update_peer(peer):
    with peers_lock:
        peers[peer] = time.time()


def check_alive_peers():
    with peers_lock:
        now = time.time()
        changed = False
        active_peers = []

        for peer, last_seen in list(peers.items()):
            if now - last_seen > EXPIRY_DURATION:
                print(f"Copy {peer} is no longer active, removing...")
                del peers[peer]
                changed = True
            else:
                active_peers.append(peer)

        if changed or active_peers:
            print("Current active copies:")
            for peer in active_peers:
                last_seen = peers[peer]
                print(f"- {peer} (last seen: {int(now - last_seen)} seconds ago)")
            print("---------------------------")


def main():
    config = parse_args()
    interface_index = get_interface(config)

    if config.protocol == "ipv6":
        sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_HOPS, struct.pack('@i', 1))
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_IF, struct.pack('@I', interface_index))
    else:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, struct.pack('@i', 1))

    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    if hasattr(socket, 'SO_REUSEPORT'):
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)

    addr = (config.multicast_address, PORT)
    sock.bind(('', PORT))

    if config.protocol == "ipv6":
        group = socket.inet_pton(socket.AF_INET6, config.multicast_address) + struct.pack('@I', interface_index)
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP, group)
    else:
        group = socket.inet_aton(config.multicast_address) + socket.inet_aton('0.0.0.0')
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, group)

    print(f"Starting application with {config.protocol} multicast address: {config.multicast_address}, "
          f"interface: {config.interface_name}, copy ID: {my_id}")

    threading.Thread(target=receive, args=(sock,), daemon=True).start()
    threading.Thread(target=send, args=(sock, addr), daemon=True).start()

    while True:
        time.sleep(1)
        check_alive_peers()


if __name__ == "__main__":
    main()