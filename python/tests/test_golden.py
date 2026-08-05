"""Golden tests — same cases as tests/golden/cases (C++ suite)."""

from __future__ import annotations

from pathlib import Path

import pytest

from mcrpc import DiscoverBuilder, Dispatcher, StatusBuilder, build_event, parse
from mcrpc.request import ParseResult
from mcrpc.version import PROTOCOL_VERSION, SDK_VERSION

GOLDEN_DIR = Path(__file__).resolve().parents[2] / "tests" / "golden" / "cases"


def _trim(s: str) -> str:
    return s.strip()


def load_cases(path: Path) -> list[dict]:
    cases: list[dict] = []
    cur: dict = {}
    st = None

    def flush() -> None:
        nonlocal cur
        if cur.get("in") is not None or cur.get("out") is not None:
            cur.setdefault("mode", "dispatch")
            cases.append(cur)
        cur = {}

    for line in path.read_text(encoding="utf-8").splitlines():
        if line.startswith("#") and (len(line) == 1 or line[1] in " \t"):
            continue
        t = _trim(line)
        if t == "---":
            flush()
            st = None
            continue
        if t == "IN":
            st = "in"
            continue
        if t == "OUT":
            st = "out"
            continue
        if t == "MODE":
            st = "mode"
            continue
        if st == "in":
            cur["in"] = t
            st = None
        elif st == "out":
            cur["out"] = t
            st = None
        elif st == "mode":
            cur["mode"] = t
            st = None
    flush()
    return cases


def _make_dispatcher() -> Dispatcher:
    d = Dispatcher()
    d.set_node_name("tracker")
    d.set_group_name("mych")
    d.register("ping", lambda req: "pong")
    d.register("relay", lambda req: "err unsupported")
    return d


@pytest.mark.parametrize("case_file", sorted(GOLDEN_DIR.glob("*.txt")), ids=lambda p: p.name)
def test_golden_file(case_file: Path) -> None:
    assert GOLDEN_DIR.is_dir(), f"golden dir missing: {GOLDEN_DIR}"
    disp = _make_dispatcher()
    for case in load_cases(case_file):
        mode = case.get("mode", "dispatch")
        if mode in ("dispatch", "ignore"):
            got = disp.dispatch(case["in"])
            if mode == "ignore":
                assert got is None, f"expected no reply for {case}"
                continue
            assert got == case["out"], f"{case_file.name}: IN={case['in']!r} EXP={case['out']!r} GOT={got!r}"
            continue
        if mode == "build_event":
            name = case["in"]
            kv = ""
            if " " in name:
                name, kv = name.split(" ", 1)
            got = build_event(name, kv or None)
            assert got == case["out"]
            continue
        if mode == "build_request":
            result, req = parse(case["in"])
            assert result == ParseResult.Ok
            rid = req.request_id if req.has_request_id else None
            from mcrpc import build_request

            got = build_request(req.target, req.command, request_id=rid)
            assert got == case["out"]
            continue
        if mode == "build_discover":
            b = DiscoverBuilder()
            b.set_node_name("tracker")
            b.add("profile", "tracker")
            b.add("fw", "test")
            b.add("protocol", PROTOCOL_VERSION)
            b.add("sdk", SDK_VERSION)
            got = b.write()
            assert got == case["out"]
            continue
        if mode == "build_status":
            s = StatusBuilder()
            s.add("name", "tracker")
            s.add("profile", "tracker")
            s.add("fw", "test")
            got = s.write()
            assert got.startswith(case["out"]) or got == case["out"]
            continue
        pytest.fail(f"unknown MODE {mode}")
