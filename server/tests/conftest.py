import itertools
import socket
import subprocess
import time

import pytest

from mock_builder import MockGame
from client import Client

SERVER_BINARY = "/mnt/gamemachine/AlbertsGameMachine/server/gameserver"
HOST = "127.0.0.1"

# Linux leaves a closed listening socket's port unavailable for a real delay
# (TIME_WAIT-adjacent behavior, well beyond what SO_REUSEADDR alone papers
# over) -- rapid-cycling a TCP server needs a fresh port each time, not the
# same one reused, or back-to-back tests intermittently fail to connect to a
# port whose previous occupant only just exited. A monotonically increasing
# counter guarantees no port is ever reused within one test run.
_next_port = itertools.count(20000)


def _wait_for_port(proc, port, timeout=5.0):
    deadline = time.time() + timeout
    while time.time() < deadline:
        if proc.poll() is not None:
            raise RuntimeError(
                "server process exited early (code %r) before it ever "
                "started listening -- check its stdout/stderr" % proc.returncode)
        try:
            with socket.create_connection((HOST, port), timeout=0.2):
                return
        except OSError:
            time.sleep(0.05)
    raise TimeoutError("server never started listening on %s:%s" % (HOST, port))


class ServerHarness:
    """A running gameserver, wired to spawn mock_game.py for "TestGame".

    Declare every mock.on(...) rule the scenario needs *before* calling
    start_game() (or send("NEWGAME,...") on a client directly, followed by
    an explicit call to arm_mock()) -- the script only gets written to disk
    at that point, which is what the spawned mock_game.py process actually
    reads. See mock_builder.py for why rules can't be added reactively once
    a game process is already running.
    """

    def __init__(self, proc, port, game_dir, mock_game):
        self._proc = proc
        self._port = port
        self._game_dir = game_dir
        self.mock_game = mock_game

    def connect(self, name, password="password"):
        return Client.connect(HOST, self._port, name, password)

    def arm_mock(self):
        self.mock_game.write_script(str(self._game_dir / "script.json"))

    def start_game(self, client, game_name="TestGame"):
        self.arm_mock()
        client.send_action("room", "NEWGAME", NewGame=game_name)

    def shutdown(self):
        self._proc.terminate()
        try:
            self._proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            self._proc.kill()
            self._proc.wait(timeout=5)


@pytest.fixture
def server(tmp_path):
    game_dir = tmp_path / "TestGame"
    game_dir.mkdir()
    mock = MockGame()

    (tmp_path / "mainloc.txt").write_text("https://example.invalid/\nrooms.xml\n")
    mock_game_py = "/mnt/gamemachine/AlbertsGameMachine/server/tests/mock_game.py"
    (tmp_path / "gameconfig.txt").write_text(
        "TestGame,https://example.invalid/,TestGameClient.xml,%s\n" % mock_game_py)

    port = next(_next_port)
    log = open(str(tmp_path / "server_stdout.log"), "w")
    proc = subprocess.Popen([SERVER_BINARY, str(tmp_path), str(port)],
                             stdout=log, stderr=subprocess.STDOUT)
    try:
        _wait_for_port(proc, port)
    except Exception:
        proc.kill()
        proc.wait(timeout=5)
        raise

    harness = ServerHarness(proc, port, game_dir, mock)
    yield harness
    harness.shutdown()
    log.close()
