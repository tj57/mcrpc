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
    build_error,
    build_event,
    build_ok,
    build_request,
    parse,
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


def test_discover_alias_normalizes_to_discovery() -> None:
    r, req = parse("all discover")
    assert r == ParseResult.Ok
    assert req.command == "discovery"
    r, req = parse("all discovery")
    assert r == ParseResult.Ok
    assert req.command == "discovery"


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
    d.register("status", lambda req: "status name=n profile=p fw=f uptime=1 rssi=0")
    d.set_node_name("tracker")
    d.set_group_name("mych")

    assert d.dispatch("all ping") == "pong"
    assert d.dispatch("other ping") is None
    assert d.dispatch("tracker nope") == "err unknown_command"
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
    d.add("profile", "tracker")
    d.add("protocol", PROTOCOL_VERSION)
    d.add("sdk", SDK_VERSION)
    disc = d.write()
    assert "protocol=1.0" in disc
    assert "sdk=1.0.0" in disc

    assert build_event("button_pressed", "count=1") == "event button_pressed count=1"
    assert build_error("timeout") == "err timeout"


def test_response_generation() -> None:
    assert build_ok() == "ok"
    assert build_ok("done") == "ok done"
    assert build_request("tracker", "gps", request_id=7) == "tracker#7 gps"
    assert build_request("n", "set", args=["led", "on"]) == "n set led on"

    assert strip_sender_prefix("Alice: tracker ping") == "tracker ping"
    assert strip_sender_prefix("tracker ping") == "tracker ping"
    assert strip_sender_prefix("group:sensors ping") == "group:sensors ping"
    r, req = parse(strip_sender_prefix("group:sensors ping"))
    assert r == ParseResult.Ok and req.address_kind == AddressKind.Group
