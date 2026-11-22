#!/usr/bin/env python3
"""Fast-forward THSSS-WASM main from the ports/thsss subtree."""

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RECORD = ROOT / "docs/migration/thsss-eagler-20261122.json"


def main():
    record = json.loads(RECORD.read_text())
    remote = record["repository"]
    if not remote.endswith(".git"):
        remote += ".git"
    command = [
        sys.executable,
        "tools/ports/check-history.py",
        "--repo",
        ".",
        "--subtree",
        record["destination"],
        "--push",
        remote,
        record["branch"],
    ]
    subprocess.run(command, cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
