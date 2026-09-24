import pytest

from mock_builder import MockGame


@pytest.fixture
def mock_game():
    """A fresh script builder for one test's scenario. See mock_builder.py."""
    return MockGame()


@pytest.fixture
def server(tmp_path, mock_game):
    """Spawns the real compiled server against a temp data directory pre-wired
    to launch mock_game.py for a "TestGame" game type, yielding an object with
    a .connect(name) method returning a client.py Client.

    Not implemented yet: this is blocked on the actual server-side redesign
    (server/GameCloset.cpp, GameBox.cpp, RoomManager.cpp) that makes NEWGAME
    spawn a process instead of dlopen-ing a .so -- see the "server changes"
    TODO.md item and .claude/server_game_interface_spec.md. Until that lands,
    only mock_game.py and MockGame itself can be tested directly (see
    test_mock_game.py), without a real server in the loop at all.
    """
    pytest.skip("server fixture is blocked on the server-side spawn/pipe changes "
                "(see TODO.md and .claude/server_game_interface_spec.md)")
