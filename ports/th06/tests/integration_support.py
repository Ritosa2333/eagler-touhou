from __future__ import annotations

import os
from pathlib import Path


def require_directory_env(name: str, description: str) -> Path:
    raw = os.environ.get(name)
    if not raw:
        raise RuntimeError(f"{name} must point to {description}")
    root = Path(raw).expanduser().resolve()
    if not root.is_dir():
        raise RuntimeError(f"{name} is not a directory: {root}")
    return root


def require_host_relay() -> tuple[Path, Path]:
    root = require_directory_env(
        "TH_EAGLER_HOST_ROOT",
        "an eagler-touhou checkout containing server/netplay-relay.mjs",
    )
    relay = root / "server" / "netplay-relay.mjs"
    if not relay.is_file():
        raise RuntimeError(f"TH_EAGLER_HOST_ROOT does not contain server/netplay-relay.mjs: {root}")
    return root, relay


def require_th07_root() -> Path:
    root = require_directory_env(
        "TH_EAGLER_TH07_ROOT",
        "a TH07 eagler checkout containing tests/netplay-countdown-audio-host.html",
    )
    fixture = root / "tests" / "netplay-countdown-audio-host.html"
    if not fixture.is_file():
        raise RuntimeError(f"TH_EAGLER_TH07_ROOT is missing {fixture.relative_to(root)}: {root}")
    return root
