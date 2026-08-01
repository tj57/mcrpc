"""mcRPC — pure-Python protocol library matching the C++ reference."""

from .builder import build_error, build_event, build_ok, build_request, prefix_request_id
from .client import PendingRequest, RequestCorrelator
from .discover import DiscoverBuilder, ParsedDiscover, parse_discover
from .dispatcher import Dispatcher
from .event import ParsedEvent, is_event_line, parse_event
from .parser import parse, strip_sender_prefix
from .request import AddressKind, ParseResult, Request
from .response import ParsedResponse, ResponseKind, parse_caps_blob, parse_response
from .status import ParsedStatus, StatusBuilder, parse_status
from .version import (
    LIBRARY_VERSION,
    PROTOCOL_VERSION,
    SDK_VERSION,
    protocol_version_code,
)

def parse_gps(raw: str | None) -> dict:
    """Parse a GPS response into HA-friendly fields (unknown keys kept in parameters)."""
    r = parse_response(raw)
    return {
        "raw": r.raw,
        "request_id": r.request_id,
        "latitude": r.parameters.get("lat"),
        "longitude": r.parameters.get("lon"),
        "altitude": r.parameters.get("alt"),
        "accuracy": r.parameters.get("acc") or r.parameters.get("hdop"),
        "satellites": r.parameters.get("sat"),
        "speed": r.parameters.get("speed"),
        "heading": r.parameters.get("heading") or r.parameters.get("course"),
        "timestamp": r.parameters.get("ts") or r.parameters.get("timestamp"),
        "status": r.parameters.get("status"),
        "parameters": r.parameters,
        "fields": r.fields,
    }


def parse_battery(raw: str | None) -> dict:
    """Parse battery / voltage / charging responses."""
    r = parse_response(raw)
    percentage = r.parameters.get("percentage")
    voltage = r.parameters.get("voltage")
    charging = r.parameters.get("charging")
    if r.kind == ResponseKind.Battery and "value" in r.parameters:
        percentage = r.parameters["value"]
    elif r.kind == ResponseKind.Voltage and "value" in r.parameters:
        voltage = r.parameters["value"]
    elif r.kind == ResponseKind.Charging and "value" in r.parameters:
        charging = r.parameters["value"]
    return {
        "raw": r.raw,
        "request_id": r.request_id,
        "percentage": percentage,
        "voltage": voltage,
        "charging": charging,
        "temperature": r.parameters.get("temp") or r.parameters.get("temperature"),
        "health": r.parameters.get("health"),
        "parameters": r.parameters,
        "fields": r.fields,
    }

__all__ = [
    "PROTOCOL_VERSION",
    "SDK_VERSION",
    "LIBRARY_VERSION",
    "protocol_version_code",
    "AddressKind",
    "ParseResult",
    "Request",
    "parse",
    "strip_sender_prefix",
    "build_request",
    "build_event",
    "build_ok",
    "build_error",
    "prefix_request_id",
    "ParsedResponse",
    "ResponseKind",
    "parse_response",
    "parse_caps_blob",
    "ParsedEvent",
    "parse_event",
    "is_event_line",
    "ParsedStatus",
    "StatusBuilder",
    "parse_status",
    "ParsedDiscover",
    "DiscoverBuilder",
    "parse_discover",
    "parse_gps",
    "parse_battery",
    "Dispatcher",
    "PendingRequest",
    "RequestCorrelator",
]
