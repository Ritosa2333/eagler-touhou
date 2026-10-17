"""Python adapter for the repository-owned workspace topology.

The JSON file is the only topology declaration. Python tooling consumes it so
Node and Python workspace tests cannot silently invent different sibling names.
"""

from __future__ import annotations

import json
import subprocess
from pathlib import Path


PROJECT = Path(__file__).resolve().parents[2]
_layout = json.loads(subprocess.run(
    ["node", str(PROJECT / "scripts" / "resolve-workspace-layout.mjs"), str(PROJECT)],
    check=True, capture_output=True, text=True, encoding="utf-8",
).stdout)
REPOSITORIES = _layout["repositories"]
WORKSPACE_ROOT = Path(_layout["root"]).resolve()


def workspace_path(repository: str, *segments: str) -> Path:
    if repository == "launcher":
        return PROJECT.joinpath(*segments)
    directory = REPOSITORIES.get(repository)
    if not isinstance(directory, str) or not directory:
        raise KeyError(f"unknown workspace repository: {repository}")
    return WORKSPACE_ROOT.joinpath(directory, *segments)
