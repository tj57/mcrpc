"""Response helpers for first-wave commands (ping/status/discover/gps/battery)."""

from __future__ import annotations

from mcrpc import (
    parse_battery,
    parse_discover,
    parse_event,
    parse_gps,
    parse_response,
    parse_status,
)
from mcrpc.response import ResponseKind


def test_ping_pong() -> None:
    r = parse_response("pong")
    assert r.kind == ResponseKind.Pong
    r = parse_response("#18 pong")
    assert r.request_id == 18


def test_status_dynamic() -> None:
    s = parse_status("status name=tracker profile=tracker fw=test mystery=42")
    assert s.fields["name"] == "tracker"
    assert s.parameters["mystery"] == 42


def test_discover() -> None:
    d = parse_discover(
        "tracker profile=tracker fw=1.2 board=lw010 protocol=1.0 sdk=1.0.0 gps=yes battery=yes"
    )
    assert d.device == "tracker"
    assert d.profile == "tracker"
    assert d.firmware == "1.2"
    assert d.board == "lw010"
    assert d.protocol == "1.0"
    assert d.sdk == "1.0.0"
    assert "gps" in d.capabilities
    assert d.features["gps"] == "yes"


def test_gps() -> None:
    g = parse_gps(
        "gps lat=50.120000 lon=19.930000 alt=12.5 sat=18 hdop=1.2 vdop=1.4 "
        "speed=1.5 heading=10 provider=gnss mystery=1"
    )
    assert g["latitude"] == 50.12
    assert g["longitude"] == 19.93
    assert g["altitude"] == 12.5
    assert g["satellites"] == 18
    assert g["accuracy"] == 1.2
    assert g["vdop"] == 1.4
    assert g["speed"] == 1.5
    assert g["heading"] == 10
    assert g["provider"] == "gnss"
    assert g["extra"]["mystery"] == 1


def test_gps_start_stop_future() -> None:
    # Future-ready: ok pending / status tokens still parse
    r = parse_response("ok pending")
    assert r.kind.name == "Ok"
    g = parse_gps("gps status=nofix")
    assert g["status"] == "nofix"


def test_battery() -> None:
    b = parse_battery("battery value=97")
    assert b["percentage"] == 97
    b = parse_battery("voltage value=3.85")
    assert b["voltage"] == 3.85
    b = parse_battery("charging value=1")
    assert b["charging"] == 1
    b = parse_battery("battery value=80 temp=24.1 health=good cycles=5 x=1")
    assert b["temperature"] == 24.1
    assert b["health"] == "good"
    assert b["cycles"] == 5
    assert b["extra"]["x"] == 1


def test_event() -> None:
    e = parse_event("event gps_fix lat=1 lon=2")
    assert e is not None
    assert e.name == "gps_fix"
    assert e.parameters["lat"] == 1


def test_format_uptime_and_short_id8():
    from mcrpc import format_uptime, short_id8
    assert format_uptime(9209) == "2h33m"
    assert format_uptime(45) == "45s"
    assert short_id8("AABBCCDDEEFF0011") == "aabbccdd"
    assert short_id8(None) == ""


def test_reply_jitter_rfc0002():
    from mcrpc import (
        BROADCAST_MIN_MS,
        BROADCAST_MAX_MS,
        reply_delay_ms,
        needs_broadcast_stagger,
        identity_hash,
    )
    assert needs_broadcast_stagger(address_kind="All")
    assert not needs_broadcast_stagger(address_kind="Named", target="ha")
    d0 = reply_delay_ms(broadcast=True, identity="aabbccdd", entropy=0)
    d1 = reply_delay_ms(broadcast=True, identity="aabbccdd", entropy=1000)
    assert BROADCAST_MIN_MS <= d0 <= BROADCAST_MAX_MS
    assert BROADCAST_MIN_MS <= d1 <= BROADCAST_MAX_MS
    assert identity_hash("button") != identity_hash("ha-peer")
    assert reply_delay_ms(broadcast=False, identity="x", entropy=50) <= 120
