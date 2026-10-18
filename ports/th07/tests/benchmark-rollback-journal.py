#!/usr/bin/env python3
"""Reproducible same-compiler before/after journal benchmark.

Read the baseline directly from Git and compile via stdin. No checkout, source
replacement, worktree copy, or dependence on an old anonymous executable.
Measures storage workloads only; never interprets these numbers as game FPS.
"""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import statistics
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
COMMON = ROOT / "third_party" / "eagler-common"


def git(*args: str) -> str:
    return subprocess.check_output(["git", *args], cwd=ROOT, text=True, encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--toolchain", choices=("native", "wasm"), default="wasm")
    parser.add_argument("--before-ref", default="HEAD")
    parser.add_argument("--repeats", type=int, default=3)
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    if args.repeats < 1 or args.repeats > 20:
        parser.error("repeats must be in 1..20")
    revision = git("rev-parse", "--verify", args.before_ref).strip()
    bench = (ROOT / "tests/rollback-journal-benchmark.cpp").read_text(encoding="utf-8")
    bench = bench.replace('#include "netplay/RollbackJournal.hpp"', '')
    sources = {}
    for label in ("before", "after"):
        def read(name: str) -> str:
            path = f"src/netplay/RollbackJournal.{name}"
            if label == "before":
                return git("show", f"{revision}:{path}")
            common_path = COMMON / ("include/eagler/netplay/RollbackJournal.hpp" if name == "hpp"
                                    else "src/netplay/RollbackJournal.cpp")
            return common_path.read_text(encoding="utf-8")
        journal_cpp = read("cpp").replace('#include "RollbackJournal.hpp"', '')
        journal_cpp = journal_cpp.replace('#include <eagler/netplay/RollbackJournal.hpp>', '')
        sources[label] = (read("hpp").replace("#pragma once", "") + "\n" +
                          journal_cpp + "\n" + bench)

    env = os.environ.copy()
    if args.toolchain == "wasm":
        emsdk = ROOT.parent / "toolchains/emsdk"
        compiler = str(emsdk / "upstream/emscripten" / ("em++.exe" if os.name == "nt" else "em++"))
        env["EM_CONFIG"] = str(emsdk / ".emscripten")
    else:
        compiler = "g++"
    report = {"baseline_revision": revision, "toolchain": args.toolchain,
              "compiler": subprocess.check_output([compiler, "--version"], cwd=ROOT, env=env, text=True).splitlines()[0],
              "flags": "-std=c++17 -O2", "runs": [], "summary": {}}
    with tempfile.TemporaryDirectory(prefix="th07-journal-bench-") as directory:
        binaries = {}
        for label, source in sources.items():
            output = Path(directory) / (label + (".cjs" if args.toolchain == "wasm" else ".exe"))
            command = [compiler, "-std=c++17", "-O2", "-x", "c++", "-", "-o", str(output)]
            if args.toolchain == "wasm":
                command += ["-sENVIRONMENT=node", "-sALLOW_MEMORY_GROWTH=1"]
            subprocess.run(command, input=source, text=True, cwd=ROOT, env=env, check=True, timeout=120)
            binaries[label] = ["node", str(output)] if args.toolchain == "wasm" else [str(output)]
        for repeat in range(args.repeats):
            for label in (("before", "after") if repeat % 2 == 0 else ("after", "before")):
                command = binaries[label] + (["--require-zero-alloc"] if label == "after" else [])
                output = subprocess.check_output(command, cwd=ROOT, text=True, timeout=90)
                rows = [json.loads(line) for line in output.splitlines() if line.startswith("{")]
                if len(rows) != 4:
                    raise AssertionError(f"missing benchmark workloads: {output}")
                report["runs"].append({"implementation": label, "repeat": repeat, "workloads": rows})
        for workload in (row["workload"] for row in report["runs"][0]["workloads"]):
            entry = {}
            for label in ("before", "after"):
                rows = [row for run in report["runs"] if run["implementation"] == label
                        for row in run["workloads"] if row["workload"] == workload]
                entry[label] = {"median_mean_ms": statistics.median(row["mean_ms"] for row in rows),
                                "median_p95_ms": statistics.median(row["p95_ms"] for row in rows),
                                "allocations_per_run": [row["allocations"] for row in rows],
                                "allocated_bytes_per_run": [row["allocated_bytes"] for row in rows]}
            entry["mean_cost_reduction_percent"] = 100 * (1 - entry["after"]["median_mean_ms"] / entry["before"]["median_mean_ms"])
            report["summary"][workload] = entry
    text = json.dumps(report, indent=2)
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(text + "\n", encoding="utf-8")
    print(json.dumps({key: value for key, value in report.items() if key != "runs"}, indent=2), flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
