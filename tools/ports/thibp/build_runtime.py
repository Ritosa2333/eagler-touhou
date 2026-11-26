"""Build the import-only THIBP Web Runtime from the pinned ph3-modern source."""

import argparse
import ast
import json
import os
import subprocess
import sys
from pathlib import Path
from tempfile import TemporaryDirectory
from zipfile import ZipFile

from prepare_launcher import adapt_shell, identity


REVISION = "12ac7141b5cdcd9cabd4479ef76e6453f19fcdf3"
ARCHIVE_SHA256 = "3d5720fcd93f48b6819622b33d72837a99b6ceb170f2bff0029764066033883b"
ADAPTER_ROOT = Path(__file__).resolve().parent
BUILD_LABEL = "kizousen-hud-fps"
COOP_LABEL = "kizousen-coop"
COOP_ROOT = ADAPTER_ROOT.with_name("thibp-mp")
COOP_PATCH = COOP_ROOT / "engine.patch"
COMMON = ADAPTER_ROOT.parents[2] / "common"
COOP_MENU_HEADER = COMMON / "include/eagler/netplay/CooperativeMenuInput.hpp"


def build_room_wire(output, env):
    emsdk = Path(env["EMSDK"])
    compiler = next(path for path in (emsdk / "upstream/emscripten/emcc.py",
                                      emsdk / "install/emscripten/emcc.py") if path.is_file())
    env = {**env, "EM_CONFIG": env.get("EM_CONFIG", str(emsdk / ".emscripten"))}
    units = ["BrowserPeerTransport", "NetplayProtocol", "NetplayCore", "NetplaySession", "SessionChannel"]
    subprocess.run([
        env["PH3_PYTHON"], str(compiler), "-std=c++20", "-O3", "-flto", "-sDEFAULT_TO_CXX=1",
        "-I", str(COMMON / "include"), "-I", str(COOP_ROOT), str(COOP_ROOT / "room-wire.cpp"),
        *(str(COMMON / "src/netplay" / (unit + ".cpp")) for unit in units),
        "-sMODULARIZE=1", "-sEXPORT_ES6=1", "-sENVIRONMENT=web",
        "-sEXPORTED_RUNTIME_METHODS=ccall,UTF8ToString,HEAPU32", "--no-entry",
        "-o", str(output / "room-wire.mjs"),
    ], env=env, check=True)
    for name in ("room.mjs", "rollback-preview.mjs"):
        (output / name).write_bytes((COOP_ROOT / name).read_bytes())


def build_rollback_module(output, env):
    sdk = Path(env["EMSDK"])
    suffix = ".exe" if os.name == "nt" else ""
    candidates = [sdk / "upstream/bin", sdk / "install/bin"]
    config = Path(env.get("EM_CONFIG", str(sdk / ".emscripten")))
    if config.is_file():
        for statement in ast.parse(config.read_text(encoding="utf-8")).body:
            if isinstance(statement, ast.Assign) and isinstance(statement.value, ast.Constant) and any(
                    isinstance(target, ast.Name) and target.id == "BINARYEN_ROOT" for target in statement.targets):
                binaryen_root = Path(statement.value.value)
                candidates[:0] = [binaryen_root / "bin", binaryen_root]
    binaryen = next((directory for directory in candidates if
                     all((directory / (tool + suffix)).is_file() for tool in ("wasm-as", "wasm-opt"))), None)
    if binaryen is None:
        raise RuntimeError("The selected Emscripten SDK must include Binaryen to build THIBP rollback")
    subprocess.run([env["PH3_PYTHON"], str(COOP_ROOT / "wasm-journal.py"), str(output / "ph3-open.wasm"),
                    str(output / "ph3-rollback.wasm"), str(binaryen)], env=env, check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("engine_archive", type=Path)
    parser.add_argument("runtime_output", type=Path)
    parser.add_argument("--coop-engine", action="store_true",
                        help="Build the experimental cooperative engine for local acceptance")
    args = parser.parse_args()
    label = COOP_LABEL if args.coop_engine else BUILD_LABEL
    output = args.runtime_output.resolve()
    if output.exists():
        parser.error("Runtime output must be a new directory")
    if identity(args.engine_archive)["sha256"] != ARCHIVE_SHA256:
        parser.error(f"Download the pinned ph3-modern ZIP for {REVISION}")

    output.parent.mkdir(parents=True, exist_ok=True)
    with TemporaryDirectory(prefix="thibp-engine-", dir=output.parent) as temp:
        with ZipFile(args.engine_archive) as source:
            source.extractall(temp)
        engine = Path(temp) / f"ph3-modern-{REVISION}"
        subprocess.run(
            ["git", "apply", "--whitespace=nowarn", str(ADAPTER_ROOT / "engine.patch")],
            cwd=engine, check=True,
        )
        if args.coop_engine:
            subprocess.run(
                ["git", "apply", "--whitespace=nowarn", str(COOP_PATCH)],
                cwd=engine, check=True,
            )
            target_header = engine / "portable/multiplayer/CooperativeMenuInput.hpp"
            target_header.write_bytes(COOP_MENU_HEADER.read_bytes())
            (target_header.parent / "ThibpInput.hpp").write_bytes((COOP_ROOT / "ThibpInput.hpp").read_bytes())
        build_environment = os.environ.copy()
        build_environment.setdefault("PH3_PYTHON", sys.executable)
        build_environment.setdefault("PH3_EXTRA_FLAGS", json.dumps(["-sWASM_LEGACY_EXCEPTIONS=0"]))
        subprocess.run(
            ["node", "tools/build-web.mjs", "--exceptions=wasm", f"--label={label}"],
            cwd=engine, env=build_environment, check=True,
        )
        runtime = engine / f"build/web-wasm-exceptions-{label}"
        if args.coop_engine:
            build_room_wire(runtime, build_environment)
            build_rollback_module(runtime, build_environment)
        adapt_shell(runtime, output, coop=args.coop_engine)
    print(f"THIBP Runtime: {output}")


if __name__ == "__main__":
    main()
