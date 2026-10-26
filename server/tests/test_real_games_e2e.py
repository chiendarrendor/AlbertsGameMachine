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
import json
import shutil
import subprocess

import jsonschema
import pytest

from conftest import HOST, SERVER_BINARY, _next_port, _wait_for_port
from client import Client

REPO_ROOT = "/mnt/gamemachine/AlbertsGameMachine"
OUTPOST_SERVER = REPO_ROOT + "/Outpost/tca/Outpost"
MOV_SERVER = REPO_ROOT + "/MerchantOfVenus/tca/MerchantOfVenus"
MOV_MAP_XML = REPO_ROOT + "/MerchantOfVenus/MerchantOfVenusMap.xml"

# Per-game and ROOMGUI body schemas (see .claude/server_game_interface_spec.md's
# Phase 3 section) -- hand-authored alongside each full-state implementation,
# but never actually wired into anything until now. Validating real captured
# output against these here is purely a regression check that the real
# server-side implementation still matches its own documented contract -- it
# says nothing about whether the Java client's own field access is correct,
# since it's checking server output, not client queries.
GAME_SCHEMAS = {
    "Outpost": REPO_ROOT + "/Outpost/OutpostSchema.json",
    "MerchantOfVenus": REPO_ROOT + "/MerchantOfVenus/MerchantOfVenusSchema.json",
}
ROOMGUI_SCHEMA = REPO_ROOT + "/roomgui/ROOMGUISchema.json"
OUTPOST_SPENDS_SEQUENCE_SCHEMA = REPO_ROOT + "/Outpost/OutpostSpendsSequenceSchema.json"


def assert_matches_schema(schema_path, instance):
    with open(schema_path) as f:
        schema = json.load(f)
    jsonschema.validate(instance=instance, schema=schema)


def assert_body_matches_schema(game_name, body):
    assert_matches_schema(GAME_SCHEMAS[game_name], body)


def recv_envelope(client, timeout=2.0):
    """See test_server_e2e.py's identical helper."""
    line = client.recv(timeout=timeout)
    msg = json.loads(line)
    assert isinstance(msg, dict) and "fullState" in msg, "not a full-state envelope: %r" % (line,)
    return msg


def room_state(fullstate, name):
    for room in fullstate["body"]["rooms"]:
        if room["name"] == name:
            return room
    return None


def inhabitant_room(fullstate, name):
    # "inhabitants" is nested per-room (see ROOMGUISchema.json), not a single
    # server-wide {name,room} list -- search each room's own roster.
    for room in fullstate["body"]["rooms"]:
        if name in room["inhabitants"]:
            return room["name"]
    return None


def drain_until_roomgui(client, predicate, max_lines=200):
    """See test_server_e2e.py's identical helper -- real games generate far
    more full-state traffic than the scripted mock, so this needs a
    generous max_lines, but the content-matching approach is the same."""
    for _ in range(max_lines):
        line = client.recv()
        try:
            msg = json.loads(line)
        except ValueError:
            continue
        if not isinstance(msg, dict) or msg.get("gui") != "ROOMGUI":
            continue
        if predicate(msg["fullState"]):
            return msg["fullState"]
    raise AssertionError("never saw a matching ROOMGUI state within %d lines" % max_lines)


def drain_until_game(client, game_name, predicate, max_lines=200):
    """Same idea as drain_until_roomgui, but for a specific game's own gui
    name -- needed for the mid-game auto-transition cascades (e.g. the whole
    TurnOrder->SupplyFill->StartResources->PromptMegaResources chain fires
    within one HandleAction call), where more than one envelope can be
    queued before the one actually worth inspecting."""
    for _ in range(max_lines):
        line = client.recv()
        try:
            msg = json.loads(line)
        except ValueError:
            continue
        if not isinstance(msg, dict) or msg.get("gui") != game_name:
            continue
        if predicate(msg["fullState"]):
            return msg["fullState"]
    raise AssertionError("never saw a matching %s state within %d lines" % (game_name, max_lines))


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
    initial_roomgui = drain_until_roomgui(alice, lambda fs: True)
    assert_matches_schema(ROOMGUI_SCHEMA, initial_roomgui["body"])
    assert initial_roomgui["body"]["myroom"]["name"] == "Great Hall"

    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom") is not None)
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")

    alice.send_action("room", "NEWGAME", NewGame=game_name)
    envelope = recv_envelope(alice)
    assert envelope["gui"] == game_name
    assert envelope["fullState"]["state"] == "InitialState"
    final_roomgui = drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom")["gameName"] == game_name)
    assert_matches_schema(ROOMGUI_SCHEMA, final_roomgui["body"])
    assert final_roomgui["body"]["myroom"]["name"] == "GameRoom"
    assert final_roomgui["body"]["myroom"]["gameName"] == game_name

    # JOIN is a self-loop back to InitialState in Outpost's XML (<cyclic
    # state="InitialState"/>), so seeing a clean full-state envelope back
    # (not an ERROR, not a hang, state still InitialState, alice now in the
    # body's players list) is real confirmation the roster-based full-state
    # rewrite and the whole spawn/dispatch loop worked correctly for a real
    # game, not just the scripted mock.
    alice.send_action(game_name, "JOIN")
    envelope = recv_envelope(alice)
    assert envelope["gui"] == game_name
    assert envelope["fullState"]["state"] == "InitialState"
    player_names = [p["name"] for p in envelope["fullState"]["body"]["players"]]
    assert "alice" in player_names
    assert_body_matches_schema(game_name, envelope["fullState"]["body"])


def test_outpost_spends_sequence_round_trip(real_games_server):
    """Drives a real 2-player Outpost game far enough to produce two real
    Spend records (each player buys zero men -- a deterministic, free
    ITEMPURCHASE spend that only needs nonzero hand value, see
    Players::CanPlayerPurchase/GetMaxPurchasable -- rather than the
    WATERMULLIGAN escape hatch, which needs a specific bad commodity draw
    and isn't reliable to trigger in an automated test), then validates the
    actual sequence range-fetch request/response against
    OutpostSpendsSequenceSchema.json through the real wire protocol -- not
    just the generated C++ method in isolation. See the "Sequence/Spend"
    design discussion (docs/Sequences.txt) for why this needed its own test:
    it's the only layer that both exercises the real socket protocol and has
    schema-validation tooling (test_real_game_join_round_trip only proves the
    wire protocol up through JOIN; outposttest already owns deep gameplay
    coverage, but has no schema-validation tooling at all).
    """
    port = real_games_server
    alice = _connect(port, "alice")
    bob = _connect(port, "bob")
    drain_until_roomgui(alice, lambda fs: True)
    drain_until_roomgui(bob, lambda fs: True)

    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom") is not None)
    drain_until_roomgui(bob, lambda fs: room_state(fs, "GameRoom") is not None)
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")
    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")
    bob.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")
    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")

    def read_both():
        """Reads exactly one game envelope on *each* of alice's and bob's own
        sockets, by real identity, never positionally -- act() below sends
        from whichever of the two is the acting player, but every successful
        action still resends to both, so both must always be drained in a
        fixed, name-bound order (never "actor_fs, other_fs", which silently
        swaps identity whenever the acting player isn't always the same one,
        exactly the bug this harness hit while writing this test)."""
        return (drain_until_game(alice, "Outpost", lambda fs: True),
                drain_until_game(bob, "Outpost", lambda fs: True))

    def act(actor, action, **params):
        actor.send_action("Outpost", action, **params)
        return read_both()

    alice.send_action("room", "NEWGAME", NewGame="Outpost")
    alice_fs, bob_fs = read_both()

    alice_fs, bob_fs = act(alice, "JOIN")
    alice_fs, bob_fs = act(bob, "JOIN")

    options = {
        "discard": False, "refinery": False, "oneupgrade": False, "activebid": False,
        "supplyharshness": 0, "robots": 1, "researchmega": False, "smallresearch": False,
        "smallmicro": False, "blinddraws": False, "stock": 1,
    }
    alice_fs, bob_fs = act(alice, "OPTIONS", **options)
    alice_fs, bob_fs = act(alice, "VALIDATESETUP")
    assert alice_fs["state"] == "ValidateSetup"

    alice_fs, bob_fs = act(alice, "LIKEOPTIONS")
    # bob's own LIKEOPTIONS is the one that makes GetPlayers().AllOk() true,
    # auto-firing STARTGAME and the whole TurnOrder->SupplyFill->
    # StartResources->PromptMegaResources auto-cascade in this same call.
    alice_fs, bob_fs = act(bob, "LIKEOPTIONS")

    # MegaResources is the only state production can rest in -- every other
    # state in this chain (PromptMegaResources included) is <auto>, so it
    # never survives to be the state a broadcast actually reports.
    while alice_fs["state"] == "MegaResources":
        current = alice if alice_fs["body"]["myPlayerId"] == alice_fs["body"]["currentTurnPlayerId"] else bob
        alice_fs, bob_fs = act(current, "NUMMEGAS", i_count=0)

    assert alice_fs["state"] == "Purchase"

    current = alice if alice_fs["body"]["myPlayerId"] == alice_fs["body"]["currentTurnPlayerId"] else bob
    waiting = bob if current is alice else alice

    # The non-current-turn player can only act out of turn order after
    # internalizing (Players::IsPlayerInternalizing) -- see INTERNALIZE's
    # <allowed> clause and Albert's own explanation of this mechanic.
    alice_fs, bob_fs = act(waiting, "INTERNALIZE")

    def no_discard_string(fs):
        """i_discardstring isn't "delete nothing" when empty -- CommodityHand::
        ApplyDeletes requires exactly one '0'/'1' per card currently in hand
        (CommodityHand.cpp:107, "Mismatched delete string length" otherwise),
        so "delete nothing" is one '0' per card in myHand, not "" ."""
        inner = fs["body"]["myHand"].strip("|")
        count = len(inner.split("-")) if inner else 0
        return "0" * count

    # Both players buy zero men -- a real, free Spend(ITEMPURCHASE) each, see
    # OutpostServer.xml's BUYMEN transition body. i_nummen=0 means
    # HandDeleter's own cost/weight checks are trivially satisfied (0 < 0 is
    # false), so this only needs CanPlayerPurchase (nonzero hand value) to
    # hold, not any specific commodity draw.
    current_fs = alice_fs if current is alice else bob_fs
    alice_fs, bob_fs = act(current, "BUYMEN", i_nummen=0, i_discardstring=no_discard_string(current_fs))
    waiting_fs = bob_fs if current is alice else alice_fs
    alice_fs, bob_fs = act(waiting, "BUYMEN", i_nummen=0, i_discardstring=no_discard_string(waiting_fs))

    # Two real Spend records should now exist -- validate the actual range-
    # fetch response, through the real wire protocol, against the schema.
    alice.send_sequence_request("Outpost", "Spends", 0, 2)
    response = recv_envelope(alice)["fullState"]
    assert response["type"] == "sequence"
    assert "error" not in response
    body = response["body"]
    assert body["sequence"] == "Spends"
    assert body["firstIndex"] == 0
    assert len(body["records"]) == 2
    for record in body["records"]:
        assert_matches_schema(OUTPOST_SPENDS_SEQUENCE_SCHEMA, record)
        assert record["spendType"] == 0  # ITEMPURCHASE, per Spend.hpp's SpendType enum

    # The error path -- requesting past the real count -- goes through
    # "error", not "body", same shape as a rejected transition, but still
    # distinguished from one by "type".
    alice.send_sequence_request("Outpost", "Spends", 0, 99)
    response = recv_envelope(alice)["fullState"]
    assert response["type"] == "sequence"
    assert "body" not in response
    assert "error" in response
