"""Resolve the explicit shared SDK for portable C++ regression tools."""
import os
from pathlib import Path
import sys


def cpp_command(workspace):
    env = os.environ.copy()
    sdk = Path(env.get("EMSDK", Path(workspace) / "toolchains/emsdk")).expanduser().resolve()
    compiler = next((sdk / layout / "emscripten/em++.py" for layout in ("install", "upstream")
                     if (sdk / layout / "emscripten/em++.py").is_file()), None)
    if compiler is None:
        raise RuntimeError(f"Set EMSDK to an installed Emscripten SDK: {sdk}")
    config = Path(env.get("EM_CONFIG", sdk / ".emscripten")).resolve()
    if not config.is_file():
        raise RuntimeError(f"Emscripten configuration is missing: {config}")
    env.update(EMSDK=str(sdk), EM_CONFIG=str(config))
    return [env.get("TH_PYTHON", sys.executable), str(compiler)], env
