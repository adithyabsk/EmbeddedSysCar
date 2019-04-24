import socket
import time
import termios
import sys
import tty
import timeout_decorator

HOST = "10.154.50.137"
PORT = 9005

command_dict = {
    "w": "$w\r",  # right
    "a": "$a\r",  # left
    "s": "$s\r",  # back
    "d": "$d\r",  # right
    " ": "$e\r",  # Enable Toggle
}

def getch():
    @timeout_decorator.timeout(1)
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
                s.connect((HOST, PORT))
                print("Server connected")
                connected = True
            except Exception as e:
                print(e)
                time.sleep(5)
        else:
            try:
                cmd = command_dict.get(getch(), None)
                if cmd is None:
                    cmd = "$p\r"
                s.sendall(cmd.encode("utf-8"))
                time.sleep(5)
            except Exception as e:
                print(e)
                print("Server not connected")
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                connected = False
                pass
            time.sleep(5)
    s.close()
