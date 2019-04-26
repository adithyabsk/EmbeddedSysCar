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
DELAY_TIME = 0.2
SPAM_P_LIMIT = 5

command_dict = {
    "w": "$w\r",  # right
    "a": "$a\r",  # left
    "s": "$s\r",  # back
    "d": "$d\r",  # right
    "p": "$p\r",  # stop car
    "+": "$+\r",  # Increment Checkpoint
    "-": "$-\r",  # Decrement Checkpoint
    "@": "$@\r",  # switch to arcade
    "#": "$#\r",  # switch to auton
    "%": "$%\r",  # start auton
    "&": "$&\r",  # stop auton
    "?": "quit",
    "t": "t",
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
    is_arcade = True
    spam_p_count = 0
    prev_cmd = None

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
                if cmd == "$@\r":
                    is_arcade = True
                elif cmd == "$#\r":
                    is_arcade = False
                elif cmd == "quit":
                    quit()
                elif (cmd is None) and is_arcade and (spam_p_count < SPAM_P_LIMIT):
                    cmd = "$p\r"
                    spam_p_count += 1
                elif cmd in ["$+\r", "$-\r"]:
                    print("entered")
                    if prev_cmd == cmd:
                        cmd = None
                    spam_p_count = SPAM_P_LIMIT
                else:
                    spam_p_count = 0

                prev_cmd = cmd

                if cmd is not None:
                    s.sendall(cmd.encode("utf-8"))
            except Exception as e:
                print("Server disconnected")
                print(e)
                connected = False
    s.close()
