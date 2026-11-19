#!/usr/bin/env python3
"""Run the narrow real-transition MP replay seek fixture, without a browser.

Requires a native build manifest with `flags` and `objects` (source/sourceSha256/
object/sha256). Sources and objects are checked before reuse. The fixture extracts
the current BrowserRuntime functions verbatim, not a second seek implementation.
Example:
  python3 tests/multiplayer/replay-prefix-seek.py --objects <build.json> \
      --out /work/artifacts/replay-prefix-seek

This uses existing ignored original assets. It does not certify a natural clear,
network transport, browser rendering, or late-join spectator checkpoints.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def extract(text, signature):
    start = text.index(signature)
    cursor = text.index("{", start) + 1
    depth = 1
    while depth:
        if text[cursor] == "{":
            depth += 1
        elif text[cursor] == "}":
            depth -= 1
        cursor += 1
    return text[start:cursor]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--objects", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()
    port = Path(__file__).resolve().parents[2]
    outer = port.parents[2]
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    manifest = json.loads(args.objects.read_text())
    for row in manifest["objects"]:
        if sha(Path(row["source"])) != row["sourceSha256"]:
            raise RuntimeError("changed production source: " + row["source"])
        if sha(Path(row["object"])) != row["sha256"]:
            raise RuntimeError("changed production object: " + row["object"])
    runtime = port / "web/cpp/sdl/BrowserRuntime.cpp"
    source = port / "tests/multiplayer/native/mp_replay_prefix_seek_probe.cpp"
    signatures = [
        "bool mp_restore_history_root(",
        "bool mp_start_playback() {",
        "bool mp_tick_playback(float fps, unsigned& simulated) {",
        "EMSCRIPTEN_KEEPALIVE int th13_mp_replay_seek_stage(unsigned stage) {",
        "EMSCRIPTEN_KEEPALIVE int th13_mp_replay_seek(unsigned frame) {",
    ]
    extracted = out / "production-playback.inc"
    extracted.write_text("\n".join(extract(runtime.read_text(), s) for s in signatures) + "\n")
    fixture = out / "driver.cpp"
    fixture.write_bytes(source.read_bytes())
    commands = [
        manifest["flags"] + ["-c", str(fixture), "-o", str(out / "driver.o")],
        ["g++", str(out / "driver.o")] + [r["object"] for r in manifest["objects"]] +
        ["-l:libfreetype.so.6", "-lm", "-o", str(out / "driver")],
    ]
    with (out / "build.log").open("w") as log:
        for command in commands:
            result = subprocess.run(command, stdout=log, stderr=subprocess.STDOUT)
            if result.returncode:
                raise SystemExit(result.returncode)
    manifest.update(
        browserRuntimeSource=str(runtime), browserRuntimeSourceSha256=sha(runtime),
        testSource=str(source), testSourceSha256=sha(source),
        driverSha256=sha(fixture),
        productionFunctionsSha256=sha(extracted), executableSha256=sha(out / "driver"),
        scope="synthetic clear API trigger; real stage transition and replay; no natural-clear claim",
    )
    (out / "build.json").write_text(json.dumps(manifest, indent=2) + "\n")
    env = dict(os.environ,
        TH13_NATIVE_ASSET_ROOT=str(outer / "artifacts/reference/raw"),
        TH13_NATIVE_REPO_ROOT=str(outer),
        TH13_NATIVE_FONT=str(port / "web/sdl-runtime/assets/msgothic.ttc"))
    command = [str(out / "driver")]
    start = time.monotonic()
    with (out / "run.log").open("w") as log:
        result = subprocess.run(command, cwd=out, env=env, stdout=log, stderr=subprocess.STDOUT)
    report = dict(command=command, exitCode=result.returncode, seconds=time.monotonic() - start)
    (out / "process.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report))
    print((out / "run.log").read_text().splitlines()[-1])
    raise SystemExit(result.returncode)


if __name__ == "__main__":
    main()
