"""Explicit module-only server selection; the original source server is default."""
from __future__ import annotations

import os
from pathlib import Path
import subprocess


def package_module_server_command(project: Path, port: int, original: list[str]) -> list[str]:
    mode = os.environ.get("EAGLER_PACKAGE_TEST_FIXTURE", "source")
    if mode not in ("source", "isolated"):
        raise ValueError("EAGLER_PACKAGE_TEST_FIXTURE must be source or isolated")
    if mode == "source":
        return original
    return ["node", str(project.resolve() / "tests/support/package-module-fixture.mjs"), str(port), "--parent-stdin"]


def stop_package_module_server(process: subprocess.Popen) -> None:
    if process.poll() is not None:
        return
    if "--parent-stdin" in process.args and process.stdin:
        process.stdin.close()
    else:
        process.terminate()
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait(timeout=5)
