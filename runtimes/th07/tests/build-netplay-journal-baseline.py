#!/usr/bin/env python3
"""Controlled browser A/B: change only rollback journal storage.

Compile two translation units via stdin and reuse the other candidate objects.
No source replacement, checkout, worktree copy, or workspace traversal.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build", default="build-web-th07-netplay")
    parser.add_argument("--output", default="build-web-th07-journal-baseline")
    parser.add_argument("--revision", default="HEAD")
    args = parser.parse_args()
    if any(not re.fullmatch(r"[A-Za-z0-9_.-]+", value) or value in (".", "..")
           for value in (args.build, args.output)) or args.build == args.output:
        parser.error("distinct simple build directory names required")
    build, destination = ROOT / args.build, ROOT / args.output
    ninja = (build / "build.ninja").read_text(encoding="utf-8").splitlines()
    cache = (build / "CMakeCache.txt").read_text(encoding="utf-8")
    if "TH_DEV_TOOLS:BOOL=ON" in cache:
        parser.error("requires the production TH_DEV_TOOLS=OFF lane")

    def rule(target: str) -> tuple[list[str], dict[str, str]]:
        start = next(index for index, line in enumerate(ninja) if line.startswith(f"build {target}: "))
        inputs = ninja[start].split(": ", 1)[1].split(" |", 1)[0].split()[1:]
        properties = {}
        for line in ninja[start + 1:]:
            if not line.startswith("  "):
                break
            name, value = line.strip().split(" = ", 1)
            properties[name] = value
        return inputs, properties

    def tokens(value: str) -> list[str]:
        return shlex.split(value.replace("$:", ":").replace("$$", "$"))

    revision = subprocess.check_output(["git", "rev-parse", "--verify", args.revision], cwd=ROOT, text=True).strip()
    def old(name: str) -> str:
        return subprocess.check_output(["git", "show", f"{revision}:src/netplay/{name}"], cwd=ROOT, text=True, encoding="utf-8")

    old_header = old("RollbackJournal.hpp").replace("#pragma once", "")
    # Current Th07RollbackState consumes a few Journal API additions that were
    # introduced after the historical storage implementation. Keep the old
    # storage/index/restore algorithm intact, but add a compile-only capability
    # surface so this controlled A/B can still replace ONLY the Journal object.
    # The equivalent-touch snapshot patch experiment is not part of this
    # baseline lane, so its legacy compatibility stub deliberately reports that
    # the operation is unsupported.
    old_header = old_header.replace(
        "    std::size_t maxBlocksPerFrame = 4096;\n",
        "    std::size_t maxBlocksPerFrame = 4096;\n"
        "    bool fastBulkCopy = false;\n"
        "    bool coalesceRestore = false;\n",
    )
    old_header = old_header.replace(
        "    std::size_t BlocksForFrame(std::uint32_t frame) const;\n",
        "    std::size_t BlocksForFrame(std::uint32_t frame) const;\n"
        "    bool AddFloatPairToSnapshots(std::uint32_t, std::uint32_t, void *, float, float) { return false; }\n"
        "    std::uint64_t RestoreCopiedBytes() const { return 0; }\n"
        "    std::uint64_t RestoreSkippedBytes() const { return 0; }\n"
        "    std::uint64_t ArenaGrowths() const { return 0; }\n",
    )
    env = os.environ.copy()
    emsdk = ROOT.parent / "toolchains/emsdk"
    env["EM_CONFIG"] = str(emsdk / ".emscripten")
    compiler = str(emsdk / "upstream/emscripten" / ("em++.exe" if os.name == "nt" else "em++"))
    destination.mkdir(parents=True, exist_ok=True)
    replacements, source_hashes = {}, {}
    for name in ("RollbackJournal.cpp", "Th07RollbackState.cpp"):
        source = old(name) if name == "RollbackJournal.cpp" else (ROOT / "src/netplay" / name).read_text(encoding="utf-8")
        source = source.replace('#include "RollbackJournal.hpp"', old_header)
        source_hashes[name] = hashlib.sha256(source.encode()).hexdigest()
        target = (f"CMakeFiles/th07.dir/third_party/eagler-common/src/netplay/{name}.o"
                  if name == "RollbackJournal.cpp"
                  else f"CMakeFiles/th07.dir/src/netplay/{name}.o")
        _, properties = rule(target)
        output = destination / f"{name}.o"
        command = [compiler, *tokens(properties.get("DEFINES", "")),
                   *tokens(properties.get("INCLUDES", "")), *tokens(properties.get("FLAGS", "")),
                   "-iquote", str(ROOT / "src/netplay"), "-x", "c++", "-", "-c", "-o", str(output)]
        subprocess.run(command, input=source, cwd=build, env=env, text=True, check=True, timeout=120)
        replacements[target] = str(output)
    objects, properties = rule("th07.html")
    command = [compiler, *tokens(properties.get("FLAGS", "")), *tokens(properties["LINK_FLAGS"]),
               *(replacements.get(obj, obj) for obj in objects), "-o", str(destination / "th07.html"),
               *tokens(properties["LINK_LIBRARIES"])]
    subprocess.run(command, cwd=build, env=env, check=True, timeout=180)
    metadata = {"journal_revision": revision, "candidate_build": args.build,
                "dev_tools": False, "source_hashes": source_hashes,
                "scope": "Only journal storage replaced; other candidate code, scheduler and libraries retained.",
                "legacy_api_compat": "bulk/coalesced fields ignored; telemetry=0; equivalent-touch snapshot patch unsupported"}
    (destination / "netplay-benchmark-build.json").write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(metadata, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
