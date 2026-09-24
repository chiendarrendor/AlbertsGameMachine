"""Validates mock_game.py's own behavior directly over a subprocess pipe, with
no C++ server involved -- proves the mock and the MockGame builder agree with
each other before anything tries to wire a real server up to them.
"""
import json
import os
import subprocess
import sys

from mock_builder import MockGame
from protocol import read_message, write_message

MOCK_GAME = os.path.join(os.path.dirname(__file__), "mock_game.py")


def spawn_mock(tmp_path, mock):
    mock.write_script(str(tmp_path / "script.json"))
    return subprocess.Popen(
        [sys.executable, MOCK_GAME, str(tmp_path)],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1,
    )


def test_happy_path_emits_then_responds(tmp_path):
    mock = MockGame()
    mock.on("handleAction", action="JOIN") \
        .emit(target="alice", message="NEWSTATE,Playing,desc") \
        .emit(target="SPECTATOR", message="LEGALACTION,JOIN") \
        .respond_ok()

    proc = spawn_mock(tmp_path, mock)
    try:
        write_message(proc.stdin, {"id": 1, "method": "handleAction",
                                    "params": {"player": "alice", "action": "JOIN"}})

        first = read_message(proc.stdout)
        assert first == {"method": "event",
                          "params": {"target": "alice", "message": "NEWSTATE,Playing,desc"}}

        second = read_message(proc.stdout)
        assert second == {"method": "event",
                           "params": {"target": "SPECTATOR", "message": "LEGALACTION,JOIN"}}

        response = read_message(proc.stdout)
        assert response == {"id": 1, "result": {}}
    finally:
        proc.stdin.close()
        assert proc.wait(timeout=5) == 0


def test_error_response_has_no_preceding_events(tmp_path):
    mock = MockGame()
    mock.on("handleAction", action="MOVE").respond_error("That is not a legal move.")

    proc = spawn_mock(tmp_path, mock)
    try:
        write_message(proc.stdin, {"id": 7, "method": "handleAction",
                                    "params": {"player": "alice", "action": "MOVE"}})
        response = read_message(proc.stdout)
        assert response == {"id": 7, "error": "That is not a legal move."}
    finally:
        proc.stdin.close()
        assert proc.wait(timeout=5) == 0


def test_crash_exits_without_responding(tmp_path):
    mock = MockGame()
    mock.on("handleAction", action="MOVE").crash()

    proc = spawn_mock(tmp_path, mock)
    write_message(proc.stdin, {"id": 1, "method": "handleAction",
                                "params": {"player": "alice", "action": "MOVE"}})
    assert read_message(proc.stdout) is None  # EOF -- the "crash" happened
    assert proc.wait(timeout=5) != 0


def test_mismatched_request_fails_loudly(tmp_path):
    mock = MockGame()
    mock.on("handleAction", action="JOIN").respond_ok()

    proc = spawn_mock(tmp_path, mock)
    write_message(proc.stdin, {"id": 1, "method": "handleAction",
                                "params": {"player": "alice", "action": "SOMETHING_ELSE"}})
    assert proc.wait(timeout=5) != 0
    stderr = proc.stderr.read()
    assert "expected a request matching" in stderr


def test_extra_request_after_script_exhausted_fails_loudly(tmp_path):
    mock = MockGame()
    mock.on("handleAction", action="JOIN").respond_ok()

    proc = spawn_mock(tmp_path, mock)
    write_message(proc.stdin, {"id": 1, "method": "handleAction",
                                "params": {"player": "alice", "action": "JOIN"}})
    assert read_message(proc.stdout) == {"id": 1, "result": {}}

    write_message(proc.stdin, {"id": 2, "method": "handleAction",
                                "params": {"player": "alice", "action": "MOVE"}})
    assert proc.wait(timeout=5) != 0
    stderr = proc.stderr.read()
    assert "unexpected request after script exhausted" in stderr


def test_malformed_writes_raw_garbage(tmp_path):
    mock = MockGame()
    mock.on("handleAction", action="JOIN").malformed("not json at all {{{")

    proc = spawn_mock(tmp_path, mock)
    try:
        write_message(proc.stdin, {"id": 1, "method": "handleAction",
                                    "params": {"player": "alice", "action": "JOIN"}})
        raw_line = proc.stdout.readline()
        assert raw_line.strip() == "not json at all {{{"
    finally:
        proc.stdin.close()
        proc.wait(timeout=5)
