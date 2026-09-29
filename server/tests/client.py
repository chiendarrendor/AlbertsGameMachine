"""Thin wrapper around one simulated player's raw TCP connection to the real
server. Client->server traffic is now the JSON envelope from Phase 2 of the
wire redesign (see .claude/server_game_interface_spec.md's Action-namespacing
section): {"namespace": ..., "action": ..., "params": {...}}, plus LOGIN's own
fixed shape reusing the same envelope under the reserved "login" namespace.
Server->client traffic (events) is untouched by Phase 2 and still today's
legacy comma-line format -- recv()/expect() below don't need to change for
that reason.
"""
import json
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
        client.send(json.dumps({"namespace": "login", "action": "LOGIN",
                                 "params": {"username": name, "password": password}}))
        return client

    def send(self, line):
        self._sock.sendall((line + "\n").encode("utf-8"))

    def send_action(self, namespace, action, **params):
        """Builds and sends the standard Action envelope -- the normal way
        to talk to the server post-Phase-2, for anything other than LOGIN/
        LOGOUTOTHER (see connect() above for those)."""
        self.send(json.dumps({"namespace": namespace, "action": action, "params": params}))

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
