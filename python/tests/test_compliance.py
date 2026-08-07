"""Compliance suite — mirrors tests/compliance/test_compliance.cpp (client-relevant parts)."""

from __future__ import annotations

from mcrpc import (
    PROTOCOL_VERSION,
    SDK_VERSION,
    AddressKind,
    DiscoverBuilder,
    Dispatcher,
    ParseResult,
    StatusBuilder,
    build_call_err,
    build_call_ok,
    build_error,
    build_event,
    build_ok,
    build_request,
    is_valid_proc,
    parse,
    parse_call_result,
    parse_discover,
    strip_sender_prefix,
)


def test_whitespace() -> None:
    r, req = parse("  tracker   ping  ")
    assert r == ParseResult.Ok
    assert req.command == "ping"
    r, req = parse("\ttracker\tping\t")
    assert r == ParseResult.Ok


def test_request_ids() -> None:
    r, req = parse("ha#42 ping")
    assert r == ParseResult.Ok and req.has_request_id and req.request_id == 42
    r, req = parse("ha #99 status")
    assert r == ParseResult.Ok and req.has_request_id and req.request_id == 99


def test_at_id_addressing() -> None:
    r, req = parse("@3CBBF74E ping")
    assert r == ParseResult.Ok
    assert req.address_kind == AddressKind.Id
    assert req.target.lower() == "3cbbf74e"
    r, req = parse("#A31C ping")
    assert r != ParseResult.Ok

    d = Dispatcher()
    d.set_node_name("node1")
    d.set_node_id("3CBBF74E")
    d.register("ping", lambda _req: "pong")
    assert d.dispatch("@3CBB ping") == "pong"
    assert d.dispatch("@DEAD ping") is None

    r, req = parse("all discover")
    assert r == ParseResult.Ok
    assert req.command == "discovery"
    r, req = parse("all discovery")
    assert r == ParseResult.Ok
    assert req.command == "discovery"


def test_call_parser_neutral() -> None:
    r, req = parse("ha call scene.morning")
    assert r == ParseResult.Ok
    assert req.command == "call"
    assert req.args == ["scene.morning"]
    assert is_valid_proc("button.pressed")
    assert is_valid_proc("ha.notify")
    assert not is_valid_proc("button_pressed")
    assert not is_valid_proc("button.press.v2")


def test_call_result_kv_only() -> None:
    assert build_call_ok() == "ok"
    assert build_call_ok(lat="50", lon="19") == "ok lat=50 lon=19"
    assert build_call_err("denied", reason="acl") == "err denied reason=acl"
    got = parse_call_result("#7 ok state=closed")
    assert got["valid"] and got["ok"] and got["fields"]["state"] == "closed"
    bad = parse_call_result("ok button pressed")
    assert bad["valid"] is False


def test_addressing() -> None:
    r, req = parse("all ping")
    assert req.address_kind == AddressKind.All
    r, req = parse("self ping")
    assert req.address_kind == AddressKind.Self
    r, req = parse("group:sensors ping")
    assert req.address_kind == AddressKind.Group
    assert req.target == "sensors"


def test_malformed() -> None:
    assert parse("")[0] == ParseResult.Empty
    assert parse("tracker")[0] == ParseResult.MissingCommand
    assert parse(None)[0] == ParseResult.Empty


def test_case() -> None:
    r, req = parse("N Ping")
    assert req.command == "ping"
    r, req = parse("n set Led ON")
    assert req.args == ["Led", "ON"]


def test_dispatch_errors_broadcast() -> None:
    d = Dispatcher()
    d.register("ping", lambda req: "pong")
    d.register("status", lambda req: "status name=n tag=p fw=f up=1s rssi=0")
    d.register("relay", lambda req: "err unsupported")
    d.register("battery", lambda req: "err unsupported")
    d.register("gps", lambda req: "err unsupported")
    d.set_node_name("tracker")
    d.set_group_name("mych")

    assert d.dispatch("all ping") == "pong"
    assert d.dispatch("other ping") is None
    assert d.dispatch("tracker nope") == "err unknown_command"
    assert d.dispatch("tracker relay") == "err unsupported"
    assert d.dispatch("tracker battery") == "err unsupported"
    assert d.dispatch("tracker gps") == "err unsupported"
    assert d.dispatch("tracker#3 ping") == "#3 pong"
    assert d.dispatch("group:mych ping") == "pong"
    assert d.dispatch("group:x ping") is None


def test_builders_and_events() -> None:
    s = StatusBuilder()
    s.add("name", "n")
    s.add("battery", 50)
    line = s.write()
    assert line.startswith("status ")
    assert "battery=50" in line

    d = DiscoverBuilder()
    d.set_node_name("tracker")
    d.add("tag", "tracker")
    d.add("fw", "fw")
    d.add_versions()
    disc = d.write()
    assert "v=1.2" in disc
    assert PROTOCOL_VERSION == "1.2"
    assert SDK_VERSION == "1.2.4"

    parsed = parse_discover(
        "button id=3cbbf74e tag=ha fw=2.11.0 v=1.2 up=1h33m caps=battery,button extra=1"
    )
    assert parsed.identity_id == "3cbbf74e"
    assert parsed.wire_version == "1.2"
    assert "extra" in parsed.fields  # unknown fields preserved / ignored by clients

    assert build_event("button.pressed", "count=1") == "event button.pressed count=1"
    assert build_error("timeout") == "err timeout"


def test_response_generation() -> None:
    assert build_ok() == "ok"
    assert build_ok("done") == "ok done"
    assert build_request("tracker", "gps", request_id=7) == "tracker#7 gps"
    assert build_request("n", "set", args=["led", "on"]) == "n set led on"

    assert strip_sender_prefix("Alice: tracker ping") == "tracker ping"
    assert strip_sender_prefix("tracker ping") == "tracker ping"
    assert strip_sender_prefix("Home Assistant: all ping") == "all ping"
    assert strip_sender_prefix("group:sensors ping") == "group:sensors ping"
    r, req = parse(strip_sender_prefix("group:sensors ping"))
    assert r == ParseResult.Ok and req.address_kind == AddressKind.Group
