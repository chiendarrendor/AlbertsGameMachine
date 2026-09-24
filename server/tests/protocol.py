"""Newline-delimited JSON-RPC-ish line protocol shared by the mock game process
and (eventually) the real GameServerMain wire format -- see
.claude/server_game_interface_spec.md for the full design.
"""
import json


def read_message(stream):
    """Read one JSON message from a line-buffered stream. Returns None at EOF."""
    line = stream.readline()
    if not line:
        return None
    line = line.strip()
    if not line:
        return read_message(stream)
    return json.loads(line)


def write_message(stream, obj):
    stream.write(json.dumps(obj) + "\n")
    stream.flush()
