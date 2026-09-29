"""End-to-end tests driving the real, compiled gameserver binary over real
sockets, with a real spawned GameProcessProxy -> mock_game.py child process
on the other side of the game<->server boundary. See
.claude/server_game_interface_spec.md's "Test infrastructure this redesign
needs" section for the design this implements.

Note on boilerplate: Room::HandleNewGame always calls SendFullState right
after creating a game, and RoomManager::HandleAction always calls
GetStatusString after every action (to refresh the room GUI's status
column) -- so every scenario below has to script those calls even though
they're not "the point" of the test. That's not incidental noise; it's
accurately reflecting what the real system actually does on the wire.
"""


def drain_until(client, expected_line, max_lines=40):
    """Reads and discards lines up to and including expected_line. Matching
    on content rather than a hardcoded line count keeps these tests robust
    to incidental traffic (e.g. exactly which INHABITANT/GUIROOM broadcasts
    a second connected client happens to generate)."""
    for _ in range(max_lines):
        line = client.recv()
        if line == expected_line:
            return
    raise AssertionError("never saw %r within %d lines" % (expected_line, max_lines))


def test_login_and_room_navigation_needs_no_game_process(server):
    # Sanity check of the fixture itself -- no NEWGAME, no mock involved.
    alice = server.connect("alice")
    assert alice.recv() == "CONNECTSTRING,Welcome to Albert's Game Server, V2.0!"
    assert alice.recv() == "WELCOME"
    alice.recv()  # NEWGUI,ROOMGUI,...
    assert alice.recv() == "GAMES, "
    assert alice.recv() == "SAVEGAMES,"
    assert alice.recv() == "GUIIAM,alice"
    drain_until(alice, "INHABITANT,alice,Great Hall")

    alice.send_action("roommanager", "NEWROOM", RoomName="Clubhouse")
    drain_until(alice, "GUIROOM,Clubhouse,,")
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="Clubhouse")
    drain_until(alice, "GAMES,TestGame")


def test_newgame_and_join_round_trip(server):
    alice = server.connect("alice")
    drain_until(alice, "INHABITANT,alice,Great Hall")
    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until(alice, "GUIROOM,GameRoom,,")
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until(alice, "INHABITANT,alice,GameRoom")

    server.mock_game.on("sendFullState", player="alice") \
        .emit(target="alice", message="LEGALACTION,JOIN") \
        .respond_ok()
    server.mock_game.on("getStatusString").respond_ok({"status": "Starting up."})
    server.mock_game.on("handleAction", action="JOIN", params={}) \
        .emit(target="alice", message="NEWSTATE,Playing,desc") \
        .emit(message="TURNORDER,alice") \
        .respond_ok()
    server.mock_game.on("getStatusString").respond_ok({"status": "In Progress."})

    server.start_game(alice)
    drain_until(alice, "NEWGUI,TestGame,https://example.invalid/,TestGameClient.xml")
    assert alice.expect("LEGALACTION,JOIN")
    drain_until(alice, "GUIROOM,GameRoom,TestGame,Starting up.")

    alice.send_action("TestGame", "JOIN")
    assert alice.expect("NEWSTATE,Playing,desc")
    assert alice.expect("TURNORDER,alice")
    drain_until(alice, "GUIROOM,GameRoom,TestGame,In Progress.")


def test_rejected_action_reaches_only_the_sender(server):
    alice = server.connect("alice")
    drain_until(alice, "INHABITANT,alice,Great Hall")
    bob = server.connect("bob")
    drain_until(bob, "INHABITANT,bob,Great Hall")
    drain_until(alice, "INHABITANT,bob,Great Hall")

    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until(alice, "GUIROOM,GameRoom,,")
    drain_until(bob, "GUIROOM,GameRoom,,")
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until(alice, "INHABITANT,alice,GameRoom")
    bob.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until(bob, "INHABITANT,bob,GameRoom")
    drain_until(alice, "INHABITANT,bob,GameRoom")

    # Every rule for the whole scenario has to be declared before start_game
    # spawns the mock process -- it reads its entire script once at spawn
    # and never reactively (see mock_builder.py). RoomManager::HandleAction's
    # post-action GUIROOM refresh loop iterates every *server-wide*
    # connection (not just this room), calling getStatusString once per
    # iteration -- with alice+bob both connected, that's twice per action,
    # including NEWGAME itself (same dispatch branch) and even a rejected
    # action (the loop runs unconditionally, since HandleAction returns void).
    server.mock_game.on("sendFullState", player="alice").respond_ok()
    server.mock_game.on("sendFullState", player="bob").respond_ok()
    server.mock_game.on("getStatusString").respond_ok({"status": "Starting up."})
    server.mock_game.on("getStatusString").respond_ok({"status": "Starting up."})
    server.mock_game.on("handleAction", action="MOVE", params={"space": 99}) \
        .respond_error("That is not a legal move.")
    server.mock_game.on("getStatusString").respond_ok({"status": "Starting up."})
    server.mock_game.on("getStatusString").respond_ok({"status": "Starting up."})

    server.start_game(alice)
    drain_until(alice, "GUIROOM,GameRoom,TestGame,Starting up.")
    drain_until(bob, "GUIROOM,GameRoom,TestGame,Starting up.")

    alice.send_action("TestGame", "MOVE", space=99)
    assert alice.expect("ERROR,That is not a legal move.")
    # bob must see nothing at all from this rejected action except the
    # ordinary post-action room-status refresh every action triggers.
    drain_until(bob, "GUIROOM,GameRoom,TestGame,Starting up.")


def test_disconnect_never_reaches_the_game_process(server):
    alice = server.connect("alice")
    drain_until(alice, "INHABITANT,alice,Great Hall")
    bob = server.connect("bob")
    drain_until(bob, "INHABITANT,bob,Great Hall")
    drain_until(alice, "INHABITANT,bob,Great Hall")

    alice.send_action("roommanager", "NEWROOM", RoomName="GameRoom")
    drain_until(alice, "GUIROOM,GameRoom,,")
    drain_until(bob, "GUIROOM,GameRoom,,")
    alice.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until(alice, "INHABITANT,alice,GameRoom")
    bob.send_action("roommanager", "CHANGEROOM", TargetRoom="GameRoom")
    drain_until(bob, "INHABITANT,bob,GameRoom")
    drain_until(alice, "INHABITANT,bob,GameRoom")

    # Every rule for the whole scenario has to be declared before start_game
    # spawns the mock process (see test_rejected_action_reaches_only_the_sender
    # for why NEWGAME needs two getStatusString rules while both clients are
    # connected; bob's later JOIN only needs one, since by then alice has
    # disconnected and HandleDisconnect already removed her from
    # RoomManager::m_Inhabitants).
    server.mock_game.on("sendFullState", player="alice").respond_ok()
    server.mock_game.on("sendFullState", player="bob").respond_ok()
    server.mock_game.on("getStatusString").respond_ok({"status": "Starting up."})
    server.mock_game.on("getStatusString").respond_ok({"status": "Starting up."})
    server.mock_game.on("handleAction", action="JOIN", params={}) \
        .emit(target="bob", message="NEWSTATE,Playing,desc") \
        .respond_ok()
    server.mock_game.on("getStatusString").respond_ok({"status": "In Progress."})

    server.start_game(alice)
    drain_until(alice, "GUIROOM,GameRoom,TestGame,Starting up.")
    drain_until(bob, "GUIROOM,GameRoom,TestGame,Starting up.")

    alice.close()  # disconnect -- must not be reported to the game process
    drain_until(bob, "DROPINHABITANT,alice")

    bob.send_action("TestGame", "JOIN")
    assert bob.expect("NEWSTATE,Playing,desc")
