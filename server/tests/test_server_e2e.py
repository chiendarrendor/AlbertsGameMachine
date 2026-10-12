"""End-to-end tests driving the real, compiled gameserver binary over real
sockets, with a real spawned GameProcessProxy -> mock_game.py child process
on the other side of the game<->server boundary. See
.claude/server_game_interface_spec.md's "Test infrastructure this redesign
needs" section for the design this implements, and its Phase 3 envelope
design for the JSON shape asserted on below.

Note on boilerplate: Room::HandleNewGame always calls SendFullState right
after creating a game, and RoomManager::HandleAction always calls
RoomManager::BroadcastRoomGuiIfChanged after every action (room-namespaced
or not) -- which, for any room with an active game, means one IsDone() RPC
per *room* (not per connected player -- a real improvement over the old
per-inhabitant GetStatusString() calls this replaced). So every scenario
below has to script those calls even though they're not "the point" of the
test. That's not incidental noise; it's accurately reflecting what the real
system actually does on the wire.
"""
import json

import pytest


def recv_envelope(client, timeout=2.0):
    """Reads one line and parses it as a full-state envelope (the same
    {"recipient":...,"gui":...,"XMLLoc":...,"XMLFile":...,"fullState":...}
    shape for both ROOMGUI and any per-game full-state -- see
    OutputPort::WrapFullStateEnvelope). Raises if the line isn't one."""
    line = client.recv(timeout=timeout)
    msg = json.loads(line)
    assert isinstance(msg, dict) and "fullState" in msg, "not a full-state envelope: %r" % (line,)
    return msg


def room_state(fullstate, name):
    for room in fullstate["rooms"]:
        if room["name"] == name:
            return room
    return None


def inhabitant_room(fullstate, name):
    for inhab in fullstate["inhabitants"]:
        if inhab["name"] == name:
            return inhab["room"]
    return None


def drain_until_roomgui(client, predicate, max_lines=40):
    """Reads and discards lines (skipping legacy non-JSON lines and ROOMGUI
    envelopes not yet matching) until a ROOMGUI envelope's fullState
    satisfies predicate, returning that fullState.
    Content-matching rather than a hardcoded line count keeps these tests
    robust to incidental traffic (e.g. a second connected client's own
    broadcasts)."""
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


LOGIN_WELCOME = "Welcome to Albert's Game Server, V3.0!"


def test_login_and_room_navigation_needs_no_game_process(server):
    # Sanity check of the fixture itself, and of the pre-login LOGIN-gui
    # mini-FSM's shape on the one true happy path (no conflicting session)
    # -- no NEWGAME, no mock involved. test_logoutother_drops_stale_session
    # below covers the conflict/LOGOUTOTHER path and the illegal-action cases.
    alice = server.raw_connect()
    assert json.loads(alice.recv()) == {
        "connectstring": LOGIN_WELCOME, "gui": "LOGIN",
        "authenticated": False, "otherconnections": -1, "error": None}

    alice.send_action("login", "AUTHENTICATE", username="alice", password="password")
    assert json.loads(alice.recv()) == {
        "connectstring": LOGIN_WELCOME, "gui": "LOGIN",
        "authenticated": True, "otherconnections": 0, "error": None}

    envelope = recv_envelope(alice)
    assert envelope["gui"] == "ROOMGUI"
    assert envelope["recipient"] == "alice"
    assert room_state(envelope["fullState"], "Great Hall") == {
        "name": "Great Hall", "gameName": "", "isDone": True, "gameTypes": [], "savedGames": []}
    assert inhabitant_room(envelope["fullState"], "alice") == "Great Hall"

    alice.send_action("roommanager", "NEWROOM", RoomName="Clubhouse")
    fullstate = drain_until_roomgui(alice, lambda fs: room_state(fs, "Clubhouse") is not None)
    # Great Hall never allows a game to start in it, so its gameTypes is
    # always empty; every other room gets the full registered list. No game
    # is attached yet, so savedGames is empty too -- it only ever reflects
    # whatever game is currently attached to *this* room.
    assert room_state(fullstate, "Clubhouse") == {
        "name": "Clubhouse", "gameName": "", "isDone": True, "gameTypes": ["TestGame"], "savedGames": []}

    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="Clubhouse")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "alice") == "Clubhouse")


def test_logoutother_drops_stale_session(server):
    # Covers the conflict path through the pre-login LOGIN-gui mini-FSM
    # (Albert + Claude, 2026-10-07): a second connection authenticating as
    # an already-logged-in name gets back otherconnections == 1 and must
    # send LOGOUTOTHER to proceed; LOGOUTOTHER is illegal before
    # authenticating at all, since there'd be no proven identity to decide
    # who to kick (Albert: allowing that "would be very poor security").
    first = server.connect("alice")
    recv_envelope(first)  # its own initial ROOMGUI, not interesting here

    second = server.raw_connect()
    second.recv()  # initial LOGIN-gui state

    second.send_action("login", "LOGOUTOTHER")
    state = json.loads(second.recv())
    assert state["authenticated"] is False
    assert state["otherconnections"] == -1
    assert state["error"]

    second.send_action("login", "AUTHENTICATE", username="alice", password="password")
    assert json.loads(second.recv()) == {
        "connectstring": LOGIN_WELCOME, "gui": "LOGIN",
        "authenticated": True, "otherconnections": 1, "error": None}

    second.send_action("login", "LOGOUTOTHER")
    assert json.loads(second.recv()) == {
        "connectstring": LOGIN_WELCOME, "gui": "LOGIN",
        "authenticated": True, "otherconnections": 0, "error": None}

    # The stale first connection is actually closed, not left dangling.
    with pytest.raises(ConnectionError):
        first.recv()

    envelope = recv_envelope(second)
    assert envelope["gui"] == "ROOMGUI"
    assert inhabitant_room(envelope["fullState"], "alice") == "Great Hall"


def test_newgame_and_join_round_trip(server):
    alice = server.connect("alice")
    drain_until_roomgui(alice, lambda fs: True)
    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom") is not None)
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")

    server.mock_game.on("sendFullState", player="alice") \
        .emit(target="alice", message={"state": "InitialState", "legalActions": ["JOIN"], "body": {}}) \
        .respond_ok()
    server.mock_game.on("isDone").respond_ok({"done": False})
    server.mock_game.on("handleAction", action="JOIN", params={}) \
        .emit(target="alice", message={"state": "Playing", "legalActions": [], "body": {"turnOrder": ["alice"]}}) \
        .emit(message="SOME_LEGACY_BROADCAST,alice") \
        .respond_ok()
    server.mock_game.on("isDone").respond_ok({"done": False})

    server.start_game(alice)
    envelope = recv_envelope(alice)
    assert envelope["gui"] == "TestGame"
    assert envelope["fullState"]["legalActions"] == ["JOIN"]
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom") == {
        "name": "GameRoom", "gameName": "TestGame", "isDone": False, "gameTypes": ["TestGame"],
        "savedGames": []})

    alice.send_action("TestGame", "JOIN")
    envelope = recv_envelope(alice)
    assert envelope["gui"] == "TestGame"
    assert envelope["fullState"]["state"] == "Playing"
    assert alice.expect("SOME_LEGACY_BROADCAST,alice")
    # No ROOMGUI resend follows: isDone/gameName/inhabitants are all
    # unchanged by JOIN, so the cache-and-diff correctly suppresses it --
    # confirmed by the isDone RPC still having been consumed (the mock
    # script would fail loudly on an unexpected request otherwise).


def test_rejected_action_reaches_only_the_sender(server):
    alice = server.connect("alice")
    drain_until_roomgui(alice, lambda fs: True)
    bob = server.connect("bob")
    drain_until_roomgui(bob, lambda fs: True)
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "bob") == "Great Hall")

    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom") is not None)
    drain_until_roomgui(bob, lambda fs: room_state(fs, "GameRoom") is not None)
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")
    # bob is also in m_Inhabitants, so alice's own CHANGEROOM broadcast the
    # same ROOMGUI update to bob too -- drain it before bob acts, or it's
    # sitting ahead of bob's own ROOMGUI update in the buffer.
    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")
    bob.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")

    # Every rule for the whole scenario has to be declared before start_game
    # spawns the mock process -- it reads its entire script once at spawn
    # and never reactively (see mock_builder.py). BroadcastFullState (inside
    # Room::HandleNewGame) calls sendFullState once per *room* inhabitant
    # (alice and bob are both in GameRoom); BroadcastRoomGuiIfChanged calls
    # isDone once per room with a game -- just once here, since it's
    # per-room now, not per-connection -- for NEWGAME and again for the
    # rejected MOVE (the post-action refresh runs unconditionally, since
    # HandleAction returns void regardless of success).
    server.mock_game.on("sendFullState", player="alice").respond_ok()
    server.mock_game.on("sendFullState", player="bob").respond_ok()
    server.mock_game.on("isDone").respond_ok({"done": False})
    server.mock_game.on("handleAction", action="MOVE", params={"space": 99}) \
        .respond_error("That is not a legal move.")
    server.mock_game.on("isDone").respond_ok({"done": False})

    server.start_game(alice)
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom")["gameName"] == "TestGame")
    drain_until_roomgui(bob, lambda fs: room_state(fs, "GameRoom")["gameName"] == "TestGame")

    alice.send_action("TestGame", "MOVE", space=99)
    assert alice.expect("ERROR,That is not a legal move.")
    # bob must see nothing at all from this rejected action -- the room's
    # game/isDone/inhabitants are unchanged, so ROOMGUI's cache-and-diff
    # means no resend reaches bob at all (a real improvement over the old
    # per-action unconditional GUIROOM broadcast this replaced).
    try:
        line = bob.recv(timeout=0.3)
        raise AssertionError("bob unexpectedly received: %r" % (line,))
    except TimeoutError:
        pass


def test_successful_action_reaches_every_room_occupant(server):
    # Covers mechanism #2 from Albert's phase-3 test-coverage audit
    # (2026-10-07): a *successful* in-room action's full-state resend
    # (DLLGame::HandleAction's roster loop, exercised directly in
    # Outpost/tests/StateMachineTest.cpp's HandleActionSendsFullStateToRoster)
    # actually reaches every occupant of the room end-to-end through real
    # sockets, not just the player who sent the action. Before this, only a
    # *rejected* action (test_rejected_action_reaches_only_the_sender, which
    # reaches nobody but the sender) and NEWGAME's own separate, server-driven
    # Room::BroadcastFullState (not HandleAction's roster loop at all) were
    # covered -- nothing proved the roster loop itself reaches a bystander.
    alice = server.connect("alice")
    drain_until_roomgui(alice, lambda fs: True)
    bob = server.connect("bob")
    drain_until_roomgui(bob, lambda fs: True)
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "bob") == "Great Hall")

    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom") is not None)
    drain_until_roomgui(bob, lambda fs: room_state(fs, "GameRoom") is not None)
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")
    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")
    bob.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")

    server.mock_game.on("sendFullState", player="alice").respond_ok()
    server.mock_game.on("sendFullState", player="bob").respond_ok()
    server.mock_game.on("isDone").respond_ok({"done": False})
    server.mock_game.on("handleAction", action="MOVE", params={"space": 5}) \
        .emit(target="alice", message={"state": "Playing", "legalActions": [], "body": {"lastMover": "alice"}}) \
        .emit(target="bob", message={"state": "Playing", "legalActions": ["MOVE"], "body": {"lastMover": "alice"}}) \
        .respond_ok()
    server.mock_game.on("isDone").respond_ok({"done": False})

    server.start_game(alice)
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom")["gameName"] == "TestGame")
    drain_until_roomgui(bob, lambda fs: room_state(fs, "GameRoom")["gameName"] == "TestGame")

    alice.send_action("TestGame", "MOVE", space=5)

    alice_envelope = recv_envelope(alice)
    assert alice_envelope["gui"] == "TestGame"
    assert alice_envelope["fullState"]["body"]["lastMover"] == "alice"

    # bob never sent anything -- the whole point is that a *successful*
    # action's roster-based full-state resend reaches every room occupant,
    # not just the actor.
    bob_envelope = recv_envelope(bob)
    assert bob_envelope["gui"] == "TestGame"
    assert bob_envelope["fullState"]["body"]["lastMover"] == "alice"
    assert bob_envelope["fullState"]["legalActions"] == ["MOVE"]
    # No ROOMGUI resend follows (same reasoning as test_newgame_and_join_round_trip):
    # isDone/gameName/inhabitants are all unchanged by MOVE, so the
    # cache-and-diff correctly suppresses it -- confirmed by the isDone RPC
    # still having been consumed (the mock script would fail loudly on an
    # unexpected request otherwise).


def test_late_joiner_gets_targeted_full_state_not_a_room_broadcast(server):
    # Covers mechanism #3 from Albert's phase-3 test-coverage audit
    # (2026-10-07): Room::AddPlayerToRoom only ever calls
    # m_pGame->SendFullState(i_Name) for the *joining* player specifically --
    # it never calls Room::BroadcastFullState() (which would resend the
    # per-game full-state to every existing occupant too, the way NEWGAME
    # does). Existing occupants should see nothing beyond the ordinary
    # ROOMGUI update reflecting the new inhabitant.
    alice = server.connect("alice")
    drain_until_roomgui(alice, lambda fs: True)
    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom") is not None)
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")

    server.mock_game.on("sendFullState", player="alice") \
        .emit(target="alice", message={"state": "InitialState", "legalActions": ["JOIN"], "body": {}}) \
        .respond_ok()
    server.mock_game.on("isDone").respond_ok({"done": False})
    # bob's own HandleInitialConnection (below) triggers its own ROOMGUI
    # recompute, which rechecks IsDone() for *every* room with an active
    # game -- GameRoom already has one at this point -- even though nothing
    # about GameRoom itself changed yet.
    server.mock_game.on("isDone").respond_ok({"done": False})
    server.mock_game.on("sendFullState", player="bob") \
        .emit(target="bob", message={"state": "InitialState", "legalActions": ["JOIN"], "body": {}}) \
        .respond_ok()
    server.mock_game.on("isDone").respond_ok({"done": False})

    server.start_game(alice)
    recv_envelope(alice)  # alice's own post-NEWGAME full-state, not the point here
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom")["gameName"] == "TestGame")

    bob = server.connect("bob")
    drain_until_roomgui(bob, lambda fs: True)
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "bob") == "Great Hall")

    bob.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")

    bob_envelope = recv_envelope(bob)
    assert bob_envelope["gui"] == "TestGame"
    assert bob_envelope["fullState"]["state"] == "InitialState"

    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")

    # alice is still in GameRoom too, but must see nothing beyond that one
    # ROOMGUI update -- no second per-game full-state, confirming the join
    # was a targeted SendFullState(bob), not a room-wide BroadcastFullState().
    try:
        line = alice.recv(timeout=0.3)
        raise AssertionError("alice unexpectedly received: %r" % (line,))
    except TimeoutError:
        pass


def test_disconnect_never_reaches_the_game_process(server):
    alice = server.connect("alice")
    drain_until_roomgui(alice, lambda fs: True)
    bob = server.connect("bob")
    drain_until_roomgui(bob, lambda fs: True)
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "bob") == "Great Hall")

    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom") is not None)
    drain_until_roomgui(bob, lambda fs: room_state(fs, "GameRoom") is not None)
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")
    # bob is also in m_Inhabitants, so alice's own CHANGEROOM broadcast the
    # same ROOMGUI update to bob too -- drain it before bob acts, or it's
    # sitting ahead of bob's own ROOMGUI update in the buffer.
    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "alice") == "GameRoom")
    bob.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")
    drain_until_roomgui(alice, lambda fs: inhabitant_room(fs, "bob") == "GameRoom")

    # See test_rejected_action_reaches_only_the_sender for why NEWGAME needs
    # one isDone rule while both clients are connected; alice's disconnect
    # is itself a lobby-relevant change (the inhabitant list shrinks) so it
    # needs its own isDone re-check, and bob's later JOIN needs one more.
    server.mock_game.on("sendFullState", player="alice").respond_ok()
    server.mock_game.on("sendFullState", player="bob").respond_ok()
    server.mock_game.on("isDone").respond_ok({"done": False})
    server.mock_game.on("isDone").respond_ok({"done": False})
    server.mock_game.on("handleAction", action="JOIN", params={}) \
        .emit(target="bob", message={"state": "Playing", "legalActions": [], "body": {}}) \
        .respond_ok()
    server.mock_game.on("isDone").respond_ok({"done": False})

    server.start_game(alice)
    drain_until_roomgui(alice, lambda fs: room_state(fs, "GameRoom")["gameName"] == "TestGame")
    drain_until_roomgui(bob, lambda fs: room_state(fs, "GameRoom")["gameName"] == "TestGame")

    alice.close()  # disconnect -- must not be reported to the game process
    drain_until_roomgui(bob, lambda fs: inhabitant_room(fs, "alice") is None)

    bob.send_action("TestGame", "JOIN")
    envelope = recv_envelope(bob)
    assert envelope["gui"] == "TestGame"
    assert envelope["fullState"]["state"] == "Playing"
