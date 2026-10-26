"""Thin wrapper around one simulated player's raw TCP connection to the real
server. Client->server traffic is the JSON envelope from Phase 2 of the wire
redesign (see .claude/server_game_interface_spec.md's Action-namespacing
section): {"namespace": ..., "action": ..., "type": ..., "params": {...}}.
"type" is "transition" for every action this harness sends -- "sequence" is
the only other value, used by a sequence range-fetch request.

Pre-login is its own tiny hand-written FSM (Albert + Claude, 2026-10-07; see
the spec doc) instead of the old single-shot LOGIN/ALREADYLOGGEDIN lines: the
server pushes one flat JSON object with "gui": "LOGIN" on connect and after
every login-phase action --
{"connectstring": ..., "gui": "LOGIN", "authenticated": bool,
 "otherconnections": -1|0|1, "error": string|null} -- and the client drives
it with AUTHENTICATE(username,password), then LOGOUTOTHER (no params needed,
since AUTHENTICATE already proved identity) only if otherconnections comes
back 1. Login is complete the instant authenticated=true/otherconnections=0
comes back; the server immediately follows with the normal post-login
ROOMGUI full-state. connect() below drives this whole exchange to
completion, since nearly every test just wants a logged-in client; see
raw_connect() for tests that need to inspect the handshake itself (e.g.
testing a rejected AUTHENTICATE, or a real username conflict).

Server->client traffic otherwise is a mix: a handful of messages stay the
old comma-line format (roommanager-level ERROR), since those were
deliberately left outside Phase 3's full-state mechanism -- but ROOMGUI and
any per-game full-state are one JSON object per line (see
test_server_e2e.py's recv_envelope/drain_until_roomgui helpers). SAVEGAMES
used to be one of these legacy lines too, but it's now folded into each
room's own object inside ROOMGUI's "savedGames" field, since it's genuinely
per-room content (which save files exist depends on which game is attached
to that room) rather than something that belongs outside the full-state
mechanism. recv()/expect() below are format-agnostic either way -- they just
return/compare raw lines -- so no change was needed there.
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
    def raw_connect(cls, host, port):
        """Opens the socket only -- no login. Returns a Client positioned to
        read the server's initial LOGIN-gui state and drive AUTHENTICATE/
        LOGOUTOTHER by hand. Most tests want connect() instead."""
        return cls(socket.create_connection((host, port)))

    @classmethod
    def connect(cls, host, port, name, password="password"):
        """Opens the socket and drives the pre-login LOGIN-gui FSM to
        completion (see module docstring) -- the normal way to get a
        logged-in client."""
        client = cls.raw_connect(host, port)
        client._login(name, password)
        return client

    def _login(self, name, password):
        self.recv()  # initial LOGIN-gui state, unauthenticated
        self.send_action("login", "AUTHENTICATE", username=name, password=password)
        state = json.loads(self.recv())
        if state.get("otherconnections") == 1:
            self.send_action("login", "LOGOUTOTHER")
            state = json.loads(self.recv())
        assert state.get("authenticated") and state.get("otherconnections") == 0, \
            "login did not complete: %r" % (state,)

    def send(self, line):
        self._sock.sendall((line + "\n").encode("utf-8"))

    def send_action(self, namespace, action, **params):
        """Builds and sends the standard Action envelope -- the normal way
        to talk to the server post-Phase-2, for anything other than LOGIN/
        LOGOUTOTHER (see connect() above for those). "type" is always
        "transition" here -- see send_sequence_request for the other value."""
        self.send(json.dumps({"namespace": namespace, "action": action,
                               "type": "transition", "params": params}))

    def send_sequence_request(self, namespace, sequence_name, start, end):
        """The other "type" value -- a read-only range-fetch request, not a
        state-mutating action. "action" doubles as the sequence name
        (docs/Sequences.txt); "start"/"end" are the requested [start, end)
        range."""
        self.send(json.dumps({"namespace": namespace, "action": sequence_name,
                               "type": "sequence", "params": {"start": start, "end": end}}))

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
