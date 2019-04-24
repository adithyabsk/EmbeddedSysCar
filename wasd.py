import socket
import time
import termios
import sys
import tty
import timeout_decorator

HOST = "10.154.11.180"
PORT = 9005

# HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
# PORT = 65432        # Port to listen on (non-privileged ports are > 1023)

RETRY_TIME = 2
DELAY_TIME = 0.1

command_dict = {
    "w": "$w\r",  # right
    "a": "$a\r",  # left
    "s": "$s\r",  # back
    "d": "$d\r",  # right
    " ": "$e\r",  # Enable Toggle
    "?": "quit",
}


def getch():
    @timeout_decorator.timeout(DELAY_TIME)
    def _getch():
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(fd)
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch

    try:
        return _getch()
    except:
        return None


if __name__ == "__main__":
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connected = False

    while True:
        if not connected:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((HOST, PORT))
                print("Server connected")
                connected = True
            except Exception as e:
                print("Failed to connect... retrying in {} seconds".format(RETRY_TIME))
                print(e)
                time.sleep(RETRY_TIME)
        else:
            try:
                cmd = command_dict.get(getch(), None)
                if cmd is None:
                    cmd = "$p\r"
                elif cmd == "quit":
                    quit()
                s.sendall(cmd.encode("utf-8"))
            except Exception as e:
                print("Server disconnected")
                print(e)
                connected = False
    s.close()
