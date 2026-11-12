#!/usr/bin/env python3
"""Compare a recorded real-asset Session with production spectator prefix replay.

The 320-frame fixture triggers a clear through a public API; it is not a natural
clear, browser, network, or GPU certification. --rebuild compiles the supplied
production object set once after header/layout changes; subsequent runs reuse its
manifest. --runtime-source supports an immutable old adapter semantic negative.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def extract(source, signature):
    start = source.index(signature)
    cursor = source.index("{", start) + 1
    depth = 1
    while depth:
        depth += (source[cursor] == "{") - (source[cursor] == "}")
        cursor += 1
    return source[start:cursor]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--objects", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--rebuild", action="store_true")
    parser.add_argument("--runtime-source", type=Path)
    args = parser.parse_args()
    port = Path(__file__).resolve().parents[2]
    outer = port.parents[2]
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=False)
    manifest = json.loads(args.objects.read_text())
    runtime = args.runtime_source or port / "web/cpp/sdl/BrowserRuntime.cpp"
    fixture_source = port / "tests/multiplayer/native/mp_spectator_prefix_replay_probe.cpp"
    source = runtime.read_text()
    signatures = ["bool mp_tick_spectator(float fps, unsigned& simulated) {"]
    if "bool mp_restore_history_root(" in source:
        signatures.insert(0, "bool mp_restore_history_root(")
    extracted = out / "production-spectator.inc"
    extracted.write_text("\n".join(extract(source, s) for s in signatures) + "\n")
    fixture = out / "driver.cpp"
    fixture.write_bytes(fixture_source.read_bytes())
    build_start = time.monotonic()
    if args.rebuild:
        (out / "objects").mkdir()
        def compile_object(item):
            index, row = item
            source_path = Path(row["source"])
            obj = out / "objects" / (str(index) + ".o")
            log = out / "objects" / (str(index) + ".log")
            command = row.get("flags", manifest["flags"]) + ["-c", str(source_path), "-o", str(obj)]
            with log.open("w") as stream:
                result = subprocess.run(command, stdout=stream, stderr=subprocess.STDOUT)
            if result.returncode:
                raise RuntimeError("compile failed: " + str(source_path) + "; " + str(log))
            return dict(source=str(source_path), sourceSha256=sha(source_path), object=str(obj), sha256=sha(obj), flags=row.get("flags", manifest["flags"]), command=command, exitCode=result.returncode)
        with ThreadPoolExecutor(max_workers=8) as pool:
            manifest["objects"] = list(pool.map(compile_object, enumerate(manifest["objects"])))
    else:
        for header in manifest.get("headers", []):
            if sha(Path(header["path"])) != header["sha256"]:
                raise RuntimeError("changed production header: " + header["path"])
        for row in manifest["objects"]:
            if sha(Path(row["source"])) != row["sourceSha256"] or sha(Path(row["object"])) != row["sha256"]:
                raise RuntimeError("changed production source/object: " + row["source"])
    commands = [manifest["flags"] + ["-c", str(fixture), "-o", str(out / "driver.o")],
                ["g++", str(out / "driver.o")] + [r["object"] for r in manifest["objects"]] + ["-l:libfreetype.so.6", "-lm", "-o", str(out / "driver")]]
    with (out / "build.log").open("w") as stream:
        for command in commands:
            result = subprocess.run(command, stdout=stream, stderr=subprocess.STDOUT)
            if result.returncode:
                raise SystemExit(result.returncode)
    headers = [port / "web/cpp/multiplayer/NetplayRuntime.hpp", port / "web/cpp/multiplayer/SpectatorJournal.hpp"]
    headers += [outer / "eagler-touhou/common/include/eagler/netplay" / name for name in ["SpectatorStagePlayback.hpp", "SpectatorStageHistory.hpp"]]
    manifest.update(browserRuntimeSource=str(runtime), browserRuntimeSourceSha256=sha(runtime),
                    testSource=str(fixture_source), testSourceSha256=sha(fixture_source),
                    driverSha256=sha(fixture), productionFunctionsSha256=sha(extracted),
                    executableSha256=sha(out / "driver"), headers=[dict(path=str(p), sha256=sha(p)) for p in headers],
                    buildSeconds=time.monotonic() - build_start,
                    scope="actual recorded Session versus production spectator; API clear fixture, no natural-clear/network/browser/GPU claim")
    (out / "build.json").write_text(json.dumps(manifest, indent=2) + "\n")
    env = dict(os.environ, TH13_NATIVE_ASSET_ROOT=str(outer / "artifacts/reference/raw"),
               TH13_NATIVE_REPO_ROOT=str(outer), TH13_NATIVE_FONT=str(port / "web/sdl-runtime/assets/msgothic.ttc"))
    command = [str(out / "driver")]
    begin = time.monotonic()
    with (out / "run.log").open("w") as stream:
        result = subprocess.run(command, cwd=out, env=env, stdout=stream, stderr=subprocess.STDOUT)
    report = dict(command=command, exitCode=result.returncode, seconds=time.monotonic() - begin)
    (out / "process.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report))
    print((out / "run.log").read_text().splitlines()[-1])
    raise SystemExit(result.returncode)


if __name__ == "__main__":
    main()
