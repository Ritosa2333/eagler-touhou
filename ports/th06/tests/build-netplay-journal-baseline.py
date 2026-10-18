#!/usr/bin/env python3
"""Build a whole-game TH06 Web baseline by replacing only rollback Journal ABI owners.

The candidate build supplies every object, library, linker flag, shell and asset.
This script recompiles the historical RollbackJournal plus the one translation
unit that owns its in-memory instance (Th06RollbackState) against the historical
header, then relinks to a separate output directory.  No source checkout or
working-tree replacement is performed.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess


ROOT = Path(__file__).resolve().parents[1]


def git(*args: str) -> str:
    return subprocess.check_output(
        ["git", *args], cwd=ROOT, text=True, encoding="utf-8"
    )


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def simple_build_path(value: str) -> bool:
    return bool(re.fullmatch(r"[A-Za-z0-9_.-]+(?:/[A-Za-z0-9_.-]+)*", value))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build", default="build/ci-web-netplay")
    parser.add_argument("--output", default="build/th06-journal-baseline")
    parser.add_argument("--revision", default="HEAD")
    args = parser.parse_args()
    if not simple_build_path(args.build) or not simple_build_path(args.output):
        parser.error("build/output must be simple relative paths")
    build = ROOT / args.build
    destination = ROOT / args.output
    if build.resolve() == destination.resolve():
        parser.error("output must differ from candidate build")
    if destination.exists():
        parser.error(f"output already exists: {destination}")
    compile_db = build / "compile_commands.json"
    link_file = build / "CMakeFiles/th06.dir/link.txt"
    object_rsp = build / "CMakeFiles/th06.dir/objects1.rsp"
    for required in (compile_db, link_file, object_rsp, build / "th06.html"):
        if not required.is_file():
            parser.error(f"candidate build is incomplete: {required}")

    revision = git("rev-parse", "--verify", args.revision).strip()
    old_header = git("show", f"{revision}:src/netplay/RollbackJournal.hpp")
    old_source = git("show", f"{revision}:src/netplay/RollbackJournal.cpp")
    state_source = (ROOT / "src/netplay/Th06RollbackState.cpp").read_text(encoding="utf-8")

    destination.mkdir(parents=True)
    generated = destination / "baseline-src"
    generated.mkdir()
    (generated / "RollbackJournal.hpp").write_text(old_header, encoding="utf-8", newline="\n")
    (generated / "RollbackJournal.cpp").write_text(old_source, encoding="utf-8", newline="\n")
    (generated / "Th06RollbackState.cpp").write_text(state_source, encoding="utf-8", newline="\n")

    rows = json.loads(compile_db.read_text(encoding="utf-8"))
    replacements: dict[str, str] = {}
    compile_records = []
    source_specs = {
        "RollbackJournal.cpp": {
            "path_fragment": "/third_party/eagler-common/src/netplay/",
            "candidate_object": "CMakeFiles/th06.dir/third_party/eagler-common/src/netplay/RollbackJournal.cpp.o",
        },
        "Th06RollbackState.cpp": {
            "path_fragment": "/src/netplay/",
            "candidate_object": "CMakeFiles/th06.dir/src/netplay/Th06RollbackState.cpp.o",
        },
    }
    for name, spec in source_specs.items():
        row = next(
            entry for entry in rows
            if Path(entry["file"]).name == name
            and spec["path_fragment"] in entry["file"].replace("\\", "/")
        )
        candidate_object = spec["candidate_object"]
        baseline_object = (destination / f"{name}.o").resolve()
        command = row["command"]
        # CMake emits exactly one output/source tail for compile_commands.
        command, count = re.subn(
            r"\s-o\s+\S+\s+-c\s+\S+\s*$",
            f" -iquote {(ROOT / 'src/netplay').resolve().as_posix()}"
            f" -o {baseline_object.as_posix()} -c {(generated / name).resolve().as_posix()}",
            command,
        )
        if count != 1:
            raise RuntimeError(f"cannot rewrite compile command for {name}: {row['command']}")
        subprocess.run(command, cwd=build, shell=True, check=True, timeout=180)
        replacements[candidate_object] = baseline_object.as_posix()
        compile_records.append({"source": name, "command": command})

    objects = object_rsp.read_text(encoding="utf-8")
    for candidate, baseline in replacements.items():
        if candidate not in objects:
            raise RuntimeError(f"candidate object missing from response file: {candidate}")
        objects = objects.replace(candidate, baseline)
    baseline_rsp = destination / "objects-baseline.rsp"
    baseline_rsp.write_text(objects, encoding="utf-8", newline="\n")

    link = link_file.read_text(encoding="utf-8").strip()
    response_tokens = ("@CMakeFiles\\th06.dir\\objects1.rsp", "@CMakeFiles/th06.dir/objects1.rsp")
    replaced = False
    for token in response_tokens:
        if token in link:
            link = link.replace(token, "@" + baseline_rsp.resolve().as_posix())
            replaced = True
            break
    if not replaced:
        raise RuntimeError("objects1.rsp token missing from link command")
    link, count = re.subn(
        r"\s-o\s+th06\.html(?:\s|$)",
        f" -o {(destination / 'th06.html').resolve().as_posix()} ",
        link,
        count=1,
    )
    if count != 1:
        raise RuntimeError("th06.html output token missing from link command")
    subprocess.run(link, cwd=build, shell=True, check=True, timeout=300)

    outputs = {}
    for extension in ("html", "js", "wasm", "data"):
        path = destination / f"th06.{extension}"
        if not path.is_file() or path.stat().st_size == 0:
            raise RuntimeError(f"baseline linker did not produce {path.name}")
        outputs[path.name] = {"bytes": path.stat().st_size, "sha256": sha256(path)}
    metadata = {
        "schema": "th06-controlled-journal-baseline/1",
        "journal_revision": revision,
        "candidate_build": args.build,
        "scope": "Only RollbackJournal and its ABI-owning Th06RollbackState translation units are recompiled; all other candidate objects/libraries/shell/assets are reused.",
        "historical_header_sha256": hashlib.sha256(old_header.encode()).hexdigest(),
        "historical_source_sha256": hashlib.sha256(old_source.encode()).hexdigest(),
        "current_state_sha256": hashlib.sha256(state_source.encode()).hexdigest(),
        "compile_records": compile_records,
        "outputs": outputs,
    }
    (destination / "journal-baseline-build.json").write_text(
        json.dumps(metadata, indent=2) + "\n", encoding="utf-8"
    )
    print(json.dumps(metadata, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
