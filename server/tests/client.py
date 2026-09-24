"""Thin wrapper around one simulated player's raw TCP connection to the real
server, speaking today's legacy comma-line client<->server protocol -- this
layer is untouched by the whole game<->server redesign this test
infrastructure exists to validate (see .claude/server_game_interface_spec.md).
"""
import selectors
import socket


class ExpectationError(AssertionError):
    pass


class Client:
    def __init__(self, sock):
        self._sock = sock
        self._buf = b""

    @classmethod
    def connect(cls, host, port, name, password="password"):
        sock = socket.create_connection((host, port))
        client = cls(sock)
        client.send("LOGIN,%s,%s" % (name, password))
        return client

    def send(self, line):
        self._sock.sendall((line + "\n").encode("utf-8"))

    def recv(self, timeout=2.0):
        while b"\n" not in self._buf:
            sel = selectors.DefaultSelector()
            sel.register(self._sock, selectors.EVENT_READ)
            try:
                if not sel.select(timeout=timeout):
                    raise TimeoutError("no line received within %rs" % timeout)
            finally:
                sel.close()
            chunk = self._sock.recv(4096)
            if not chunk:
                raise ConnectionError("connection closed")
            self._buf += chunk
        line, _, self._buf = self._buf.partition(b"\n")
        return line.decode("utf-8")

    def expect(self, expected_line, timeout=2.0):
        actual = self.recv(timeout=timeout)
        if actual != expected_line:
            raise ExpectationError("expected %r, got %r" % (expected_line, actual))
        return actual

    def close(self):
        self._sock.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc_info):
        self.close()
