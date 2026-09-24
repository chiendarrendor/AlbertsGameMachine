#!/usr/bin/env python3
"""Scriptable mock game process for server integration tests.

Speaks the stdio + newline-delimited JSON-RPC-ish protocol any real
GameServerMain-based game speaks (see .claude/server_game_interface_spec.md),
but instead of running real game rules, it follows a script: a JSON list of
rules, one per expected incoming request, in strict order. Tests build this
script with the fluent MockGame builder in mock_builder.py -- see that module
for the authoring-side API -- and it's written to <dataDir>/script.json before
this process is spawned (dataDir is argv[1], the same launch-time argument
every real game process gets).

Each rule looks like:
    {
      "expect": {"method": "handleAction", "params": {"action": "JOIN"}},
      "emit": [{"target": "alice", "message": "NEWSTATE,..."}],
      "then": {"result": {}}
    }

"params" in "expect" only constrains the keys it names -- extra keys on the
real request are ignored. "then" is exactly one of:
    {"result": {...}}    -- normal successful completion
    {"error": "..."}     -- RPC-level error, per "ERROR is a special case"
    {"crash": true}      -- exit immediately without responding
    {"hang": true}       -- never respond (block forever)
    {"malformed": "..."} -- write this exact (invalid) text instead of a response

A request that doesn't match the next expected rule, or arrives after the
script is exhausted, is a test-authoring bug, not a thing to paper over -- this
fails loudly (stderr + nonzero exit) rather than hanging silently, so a broken
test shows up as a clear failure instead of a timeout with no explanation.
"""
import json
import os
import sys
import time

from protocol import read_message, write_message


def load_script(data_dir):
    script_path = os.path.join(data_dir, "script.json")
    with open(script_path) as f:
        return json.load(f)


def matches(request, expect):
    if request.get("method") != expect.get("method"):
        return False
    got_params = request.get("params", {}) or {}
    for key, want_value in expect.get("params", {}).items():
        if got_params.get(key) != want_value:
            return False
    return True


def fail(message):
    sys.stderr.write("mock_game: " + message + "\n")
    sys.stderr.flush()
    sys.exit(1)


def run(data_dir, in_stream, out_stream):
    rules = load_script(data_dir)

    for rule in rules:
        request = read_message(in_stream)
        if request is None:
            fail("stdin closed early -- expected a %r request" % (rule["expect"],))

        expect = rule["expect"]
        if not matches(request, expect):
            fail("expected a request matching %r, got %r" % (expect, request))

        for event in rule.get("emit", []):
            write_message(out_stream, {"method": "event", "params": event})

        then = rule.get("then", {"result": {}})
        req_id = request.get("id")

        if "crash" in then:
            sys.exit(1)
        if "hang" in then:
            while True:
                time.sleep(3600)
        if "malformed" in then:
            out_stream.write(then["malformed"] + "\n")
            out_stream.flush()
            continue
        if "error" in then:
            write_message(out_stream, {"id": req_id, "error": then["error"]})
            continue
        write_message(out_stream, {"id": req_id, "result": then.get("result", {})})

    # Script exhausted. A clean shutdown looks like the server closing our
    # stdin; anything else arriving is a script that didn't cover everything
    # the test scenario actually does.
    while True:
        request = read_message(in_stream)
        if request is None:
            return
        fail("unexpected request after script exhausted: %r" % (request,))


def main():
    if len(sys.argv) != 2:
        fail("usage: mock_game.py <dataDir>")
    run(sys.argv[1], sys.stdin, sys.stdout)


if __name__ == "__main__":
    main()
