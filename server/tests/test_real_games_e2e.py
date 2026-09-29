"""End-to-end happy-path tests against the *real*, statically-linked game
processes (outpostserver, merchantofvenusserver) -- not mock_game.py. This
is the validation .claude/server_game_interface_spec.md's Phase 1B section
used to flag as missing ("nothing built so far has been validated against
a real game process"). Deliberately a thin smoke test, not a rules test --
MoVunittests.exe/outposttest already own exhaustive rules coverage
in-process; this only needs to prove the real wire protocol (spawn, JOIN,
roster-based LEGALACTION, status refresh) works end-to-end through the
real compiled gameserver binary.
"""
import shutil
import subprocess

import pytest

from conftest import HOST, SERVER_BINARY, _next_port, _wait_for_port
from client import Client

REPO_ROOT = "/mnt/gamemachine/AlbertsGameMachine"
OUTPOST_SERVER = REPO_ROOT + "/Outpost/tca/Outpost"
MOV_SERVER = REPO_ROOT + "/MerchantOfVenus/tca/MerchantOfVenus"
MOV_MAP_XML = REPO_ROOT + "/MerchantOfVenus/MerchantOfVenusMap.xml"


def drain_until(client, expected_line, max_lines=200):
    """See test_server_e2e.py's identical helper -- real games generate far
    more LEGALACTION/state traffic than the scripted mock, so this needs a
    generous max_lines, but the content-matching approach is the same."""
    for _ in range(max_lines):
        line = client.recv()
        if line == expected_line:
            return
    raise AssertionError("never saw %r within %d lines" % (expected_line, max_lines))


@pytest.fixture
def real_games_server(tmp_path):
    (tmp_path / "mainloc.txt").write_text("https://example.invalid/\nrooms.xml\n")
    (tmp_path / "gameconfig.txt").write_text(
        "Outpost,https://example.invalid/,OutpostClient.xml,%s\n"
        "MerchantOfVenus,https://example.invalid/,MerchantOfVenusClient.xml,%s\n"
        % (OUTPOST_SERVER, MOV_SERVER))

    # MerchantOfVenusSet's constructor reads <dataDir>/MerchantOfVenusMap.xml
    # at spawn time -- GameCloset creates <dataDir> itself (serverDataRoot/
    # name), but doesn't know to seed this game-private data file, so the
    # test has to (mirroring what `install` normally copies into
    # data/MerchantOfVenus/ -- see MerchantOfVenus/tca/Makefile).
    mov_dir = tmp_path / "MerchantOfVenus"
    mov_dir.mkdir()
    shutil.copy(MOV_MAP_XML, mov_dir / "MerchantOfVenusMap.xml")

    port = next(_next_port)
    log = open(str(tmp_path / "server_stdout.log"), "w")
    proc = subprocess.Popen([SERVER_BINARY, str(tmp_path), str(port)],
                             stdout=log, stderr=subprocess.STDOUT)
    try:
        _wait_for_port(proc, port)
        yield port
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait(timeout=5)
        log.close()


def _connect(port, name):
    return Client.connect(HOST, port, name)


@pytest.mark.parametrize("game_name", ["Outpost", "MerchantOfVenus"])
def test_real_game_join_round_trip(real_games_server, game_name):
    port = real_games_server
    alice = _connect(port, "alice")
    drain_until(alice, "INHABITANT,alice,Great Hall")

    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until(alice, "GUIROOM,GameRoom,,")
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until(alice, "INHABITANT,alice,GameRoom")

    alice.send_action("room", "NEWGAME", NewGame=game_name)
    drain_until(alice, "GUIROOM,GameRoom,%s,Starting up." % game_name)

    # JOIN is a self-loop back to InitialState in both games' XML (<cyclic
    # state="InitialState"/>), so status stays "Starting up." afterward --
    # seeing this line again (not an ERROR, not a hang, not a differently
    # worded status) is real confirmation the roster-based LEGALACTION
    # rewrite and the whole spawn/dispatch loop worked correctly for a real
    # game, not just the scripted mock.
    alice.send_action(game_name, "JOIN")
    drain_until(alice, "GUIROOM,GameRoom,%s,Starting up." % game_name)
