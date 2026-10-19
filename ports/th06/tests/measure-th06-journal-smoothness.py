#!/usr/bin/env python3
"""Serial paired TH06 Journal browser experiments with frozen runtime hashes."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import statistics
import subprocess
import sys
import time


ROOT = Path(__file__).resolve().parents[1]
SUITES = {
    "journal": {
        "old": "dense2-oldjournal-fixed",
        "new": "dense2-newjournal-fixed",
        "runs": "dense2-runs-fixed",
    },
    "backends": {
        "runs": "dense2-runs-fixed",
        "bulk": "dense2-runs-bulk-fixed",
        "restore": "dense2-runs-restore-fixed",
        "both": "dense2-runs-both-fixed",
    },
    "frontier": {
        "runs": "dense2-runs-frontier-fixed",
        "restore": "dense2-runs-restore-frontier-fixed",
        "both": "dense2-runs-both-frontier-fixed",
        "live": "dense2-live-frontier-fixed",
    },
}
PERF_RE = re.compile(
    r"dense performance: logicFps=([0-9.]+)/([0-9.]+) "
    r"rafP95=([0-9.]+)/([0-9.]+) rafP99=([0-9.]+)/([0-9.]+) "
    r"rafMax=([0-9.]+)/([0-9.]+) rafGt50=(\d+)/(\d+) elapsed=([0-9.]+)s"
)
SNAPSHOT_RE = re.compile(r"dense rollback: bytes=(\d+)/(\d+) blocks=(\d+)/(\d+)")


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def identity(directory: Path) -> dict[str, str]:
    return {name: digest(directory / name) for name in ("th06.html", "th06.js", "th06.wasm", "th06.data")}


def parse_metrics(text: str) -> dict:
    perf = PERF_RE.search(text)
    snapshot = SNAPSHOT_RE.search(text)
    if not perf or not snapshot:
        raise RuntimeError("missing dense performance/snapshot summary")
    p, s = perf.groups(), snapshot.groups()
    return {
        "logic_fps": [float(p[0]), float(p[1])],
        "raf_p95_ms": [float(p[2]), float(p[3])],
        "raf_p99_ms": [float(p[4]), float(p[5])],
        "raf_max_ms": [float(p[6]), float(p[7])],
        "raf_gt50": [int(p[8]), int(p[9])],
        "sample_elapsed_seconds": float(p[10]),
        "snapshot_bytes": [int(s[0]), int(s[1])],
        "snapshot_blocks": [int(s[2]), int(s[3])],
    }


def summarize(runs: list[dict], modes: dict[str, str]) -> dict:
    result = {}
    for label in modes:
        rows = [r for r in runs if r["implementation"] == label and r["status"] == "PASS"]
        if not rows:
            result[label] = {"status": "NO_PASS"}
            continue
        m = [row["metrics"] for row in rows]
        result[label] = {
            "status": "PASS",
            "passes": len(rows),
            "weak_median_logic_fps": statistics.median(x["logic_fps"][1] for x in m),
            "weak_median_raf_p95_ms": statistics.median(x["raf_p95_ms"][1] for x in m),
            "weak_median_raf_p99_ms": statistics.median(x["raf_p99_ms"][1] for x in m),
            "weak_median_raf_max_ms": statistics.median(x["raf_max_ms"][1] for x in m),
            "weak_median_raf_gt50": statistics.median(x["raf_gt50"][1] for x in m),
            "median_snapshot_bytes": statistics.median(x["snapshot_bytes"][1] for x in m),
            "median_snapshot_blocks": statistics.median(x["snapshot_blocks"][1] for x in m),
        }
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rounds", type=int, choices=range(1, 6), default=3)
    parser.add_argument("--suite", choices=tuple(SUITES), default="journal")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    output = args.output.resolve()
    if output.exists():
        parser.error(f"output already exists: {output}")
    host = os.environ.get("TH_EAGLER_HOST_ROOT")
    if not host:
        parser.error("TH_EAGLER_HOST_ROOT is required")
    output.mkdir(parents=True)
    modes = SUITES[args.suite]
    runtime_dirs = {
        label: ROOT / ("build/th06-journal-baseline-b" if label == "old" else "build/ci-web-netplay")
        for label in modes
    }
    identities = {k: identity(v) for k, v in runtime_dirs.items()}
    report = {
        "schema": "th06-journal-paired-browser/1",
        "settings": {"suite": args.suite, "rounds": args.rounds, "delay_ms": 20, "jitter_ms": 0, "drop_every": 7, "cpu_rate": 4, "frames": 300},
        "runtime_identities": identities,
        "runs": [],
        "summary": {},
    }
    for round_index in range(args.rounds):
        order = list(modes) if round_index % 2 == 0 else list(reversed(modes))
        for label in order:
            started = time.monotonic()
            child = subprocess.run(
                [sys.executable, str(ROOT / "tests/netplay-browser-smoke.py"), modes[label]],
                cwd=ROOT,
                env={**os.environ, "TH_EAGLER_HOST_ROOT": host},
                text=True,
                capture_output=True,
                timeout=180,
            )
            text = child.stdout + ("\n--- stderr ---\n" + child.stderr if child.stderr else "")
            (output / f"r{round_index + 1}-{label}.log").write_text(text, encoding="utf-8")
            record = {
                "round": round_index + 1,
                "implementation": label,
                "mode": modes[label],
                "exit_code": child.returncode,
                "wall_seconds": round(time.monotonic() - started, 3),
                "status": "PASS" if child.returncode == 0 else "FAIL",
            }
            try:
                record["metrics"] = parse_metrics(text)
            except Exception as error:
                record["status"] = "FAIL"
                record["parse_error"] = str(error)
            if identity(runtime_dirs[label]) != identities[label]:
                record["status"] = "FAIL"
                record["identity_error"] = "runtime changed during experiment"
            report["runs"].append(record)
            report["summary"] = summarize(report["runs"], modes)
            (output / "summary.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
            print(json.dumps(record), flush=True)
    report["summary"] = summarize(report["runs"], modes)
    (output / "summary.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report["summary"], indent=2), flush=True)
    return 0 if all(run["status"] == "PASS" for run in report["runs"]) else 1


if __name__ == "__main__":
    raise SystemExit(main())
