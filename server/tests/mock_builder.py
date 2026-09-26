"""Fluent builder for a mock_game.py script, authored directly inside a test.

    mock = MockGame()
    mock.on("handleAction", action="JOIN") \\
        .emit(target="alice", message="NEWSTATE,Playing,...") \\
        .emit(target="SPECTATOR", message="LEGALACTION,JOIN") \\
        .respond_ok()

Rules are matched in the order declared, one per incoming request -- so the
*entire* scenario for a test must be declared before the game process that
will read it gets spawned (i.e. before whatever client action triggers
NEWGAME/LOADGAME). There's no way to add a rule reactively once the mock
process is already running; if a test needs that, it isn't supported yet (see
.claude/server_game_interface_spec.md's test-infrastructure section, which
deliberately scoped strict-sequence matching as the first cut).
"""
import json


class Rule:
    def __init__(self, method, params):
        self._method = method
        self._params = params
        self._emits = []
        self._then = None

    def emit(self, message, target=None):
        event = {"message": message}
        if target is not None:
            event["target"] = target
        self._emits.append(event)
        return self

    def respond_ok(self, result=None):
        self._then = {"result": result if result is not None else {}}
        return self

    def respond_error(self, message):
        self._then = {"error": message}
        return self

    def crash(self):
        self._then = {"crash": True}
        return self

    def hang(self):
        self._then = {"hang": True}
        return self

    def malformed(self, raw_text):
        self._then = {"malformed": raw_text}
        return self

    def to_dict(self):
        if self._then is None:
            # A forgotten terminal call should still produce a script that
            # runs (and fails the test's later assertions loudly) rather than
            # a mock that silently hangs forever on this request.
            self._then = {"result": {}}
        return {
            "expect": {"method": self._method, "params": self._params},
            "emit": self._emits,
            "then": self._then,
        }


class MockGame:
    def __init__(self):
        self._rules = []

    def on(self, method, **params):
        rule = Rule(method, params)
        self._rules.append(rule)
        return rule

    def write_script(self, path):
        with open(path, "w") as f:
            json.dump([r.to_dict() for r in self._rules], f)
