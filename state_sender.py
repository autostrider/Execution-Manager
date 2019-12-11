import socket
import os

SOCK_ADDR = '/tmp/state_setter'

if __name__ == "__main__":
  if os.path.exists(SOCK_ADDR):
    sock_client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock_client.connect(SOCK_ADDR)
    print("Ctrl+C to quit.")
    try:
      while True:
          val = input()
          sock_client.send(val.encode())
    except KeyboardInterrupt:
        print("Shutting down")
        sock_client.close()
  else:
    print("Couldn't connect to socket")
