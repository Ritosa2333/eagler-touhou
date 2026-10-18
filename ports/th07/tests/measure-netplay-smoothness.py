#!/usr/bin/env python3
"""Freeze one runtime/fixture, then run serial, paired netplay experiments.

No production files are changed. RTC impairment is application send delay, not
wire RTT; its actually delivered delay is reported. CPU throttling is not a
phone model. Raw failures are retained and never silently rerun as successes.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[1]


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def summarize(report: dict) -> dict:
    result = {"status": report.get("status"), "error": report.get("error"), "peers": []}
    for peer in report.get("peers", []):
        costs = peer.get("costs", {})
        impairment = peer.get("input_impairment") or {}
        sent = impairment.get("sent", 0)
        result["peers"].append({
            "simulation_fps": round(peer.get("simulation_fps", 0), 2),
            "present_p95_ms": round(peer.get("presentGapMs", {}).get("p95_ms", 0), 2),
            "present_p99_ms": round(peer.get("presentGapMs", {}).get("p99_ms", 0), 2),
            "present_max_ms": round(peer.get("presentGapMs", {}).get("max_ms", 0), 2),
            "advance_p99_ms": round(peer.get("visible_advance", {}).get("p99_ms", 0), 2),
            "driver_p95_ms": round(peer.get("driverMs", {}).get("p95_ms", 0), 2),
            "draw_p95_ms": round(peer.get("drawMs", {}).get("p95_ms", 0), 2),
            "rollback": peer.get("rollback"), "resimulated": peer.get("resimulated"),
            # Full event traces remain in each raw report. Keep console and
            # comparison summaries bounded so diagnostics do not overwhelm MCP.
            "costs": {k: v for k, v in costs.items() if k not in ("longFrames", "tracePrevious")},
            "injected_mean_delivered_ms": round(impairment.get("deliveredDelayTotalMs", 0) / sent, 2) if sent else None,
            "injected_max_timer_overrun_ms": impairment.get("maxTimerOverrunMs"),
            "hashes": peer.get("hashes"),
        })
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build", default="build-web-th07-netplay")
    parser.add_argument("--rounds", type=int, choices=range(1, 6), default=1)
    parser.add_argument("--frames", type=int, default=900)
    parser.add_argument("--cpu-rate", type=float, default=4)
    parser.add_argument("--browser-channel", choices=("default", "chromium", "msedge", "chrome"), default="default")
    parser.add_argument("--players", type=int, choices=(2, 3), default=2)
    parser.add_argument("--delays", default="3,6")
    parser.add_argument("--one-way-ms", type=float, default=50)
    parser.add_argument("--jitter-ms", type=float, default=10)
    parser.add_argument("--dense-bullets", type=int, default=900)
    parser.add_argument("--policies", default="always,demand")
    parser.add_argument("--snapshot-layout", choices=("objects", "runs"), default="objects")
    parser.add_argument("--snapshot-copy", choices=("wasm", "bulk"), default="wasm")
    parser.add_argument("--snapshot-restore", choices=("sequential", "coalesced"), default="sequential")
    parser.add_argument("--checkpoint-frames", type=int, choices=range(1, 9), default=2)
    parser.add_argument("--bridge-drag", action="store_true")
    parser.add_argument("--curved-bullets", action="store_true")
    parser.add_argument("--transport", choices=("rtc", "relay"), default="rtc")
    parser.add_argument("--real-stage", type=int, choices=range(1, 7))
    parser.add_argument("--difficulty", type=int, choices=range(4), default=1)
    parser.add_argument("--auto-profile", action="store_true")
    parser.add_argument("--freeze-only", action="store_true")
    parser.add_argument("--include-ogg", action="store_true", help="Freeze the explicitly scoped TH07 OGG directory for audio-on runs")
    parser.add_argument("--limit-presentation-60", action="store_true")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    delays = [int(value) for value in args.delays.split(",")]
    policies = args.policies.split(",")
    if any(value < 0 or value > 12 for value in delays) or any(value not in ("always", "demand", "frontier", "off") for value in policies):
        parser.error("invalid delay/policy list")
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    fixture = output / "frozen"
    build = ROOT / args.build
    sources = {build / f"th07.{extension}": fixture / "runtime" / f"th07.{extension}"
               for extension in ("html", "js", "wasm", "data")}
    sources[build / "CMakeCache.txt"] = fixture / "runtime/CMakeCache.txt"
    for name in ("netplay-performance-browser.py", "netplay-performance-browser-host.html", "netplay-input-latency-probe.cjs", "netplay-audio-probe.cjs"):
        sources[ROOT / "tests" / name] = fixture / "tests" / name
    sources[ROOT / "third_party/eagler-common/testkit/rtc-input-impairment.cjs"] = \
        fixture / "third_party/eagler-common/testkit/rtc-input-impairment.cjs"
    if args.include_ogg:
        for source in (ROOT / "assets-ogg/bgm-ogg").glob("th07_*.ogg"):
            sources[source] = fixture / "assets-ogg/bgm-ogg" / source.name
    sources[ROOT / "assets/msgothic.ttc"] = fixture / "assets/msgothic.ttc"
    identities = {str(path.relative_to(ROOT)): digest(path) for path in sources}
    for source, destination in sources.items():
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(source, destination)
        if digest(destination) != identities[str(source.relative_to(ROOT))]:
            raise RuntimeError(f"input changed while freezing: {source}")
    if identities != {str(path.relative_to(ROOT)): digest(path) for path in sources}:
        raise RuntimeError("build changed while freezing; no measurements accepted")
    # The shadow fixture owns its immutable HTTP files, while the relay remains
    # the shared implementation. Freeze/check that small script as provenance.
    relay = ROOT.parent / "eagler-touhou/server/netplay-relay.mjs"
    relay_identity = digest(relay)
    bootstrap = fixture / "run.py"
    bootstrap.write_text(
        "import importlib.util\nfrom pathlib import Path\n"
        "s=importlib.util.spec_from_file_location('frozen_perf',Path(__file__).parent/'tests/netplay-performance-browser.py')\n"
        "m=importlib.util.module_from_spec(s)\ns.loader.exec_module(m)\n"
        f"m.RELAY_ROOT=Path({str(relay.parents[1])!r})\n"
        "raise SystemExit(m.main())\n", encoding="utf-8")
    result = {"schema": "th07-netplay-paired-experiment/1", "identities": identities,
              "relay_sha256": relay_identity, "settings": {k: str(v) if isinstance(v, Path) else v for k, v in vars(args).items()},
              "runs": []}
    if args.freeze_only:
        (output / "summary.json").write_text(json.dumps(result, indent=2), encoding="utf-8")
        print(f"FROZEN {bootstrap}", flush=True)
        return 0
    for repeat in range(args.rounds):
        for delay in delays:
            ordered = policies if repeat % 2 == 0 else policies[::-1]
            for policy in ordered:
                label = f"r{repeat + 1}-d{delay}-{policy}"
                raw = output / f"{label}.json"
                command = [sys.executable, str(bootstrap), "--build", "runtime",
                           "--delay-ms", str(int(args.one_way_ms) if args.transport == "relay" else 0),
                           "--jitter-ms", str(int(args.jitter_ms) if args.transport == "relay" else 0), "--players", str(args.players),
                           "--frames", str(args.frames), "--cpu-rate", str(args.cpu_rate),
                           "--browser-channel", args.browser_channel,
                           "--slow-player", str(args.players - 1), "--input-delay", str(delay),
                           "--dense-bullets", str(args.dense_bullets), "--touch-stream", "delta",
                           "--touch-workload", "default" if args.bridge_drag else "variable", "--snapshot-policy", "always" if policy == "off" else policy,
                           "--snapshot-layout", args.snapshot_layout,
                           "--snapshot-copy", args.snapshot_copy,
                           "--snapshot-restore", args.snapshot_restore,
                           "--checkpoint-frames", str(args.checkpoint_frames),
                           "--timeout", "100", "--output", str(raw)]
                if policy == "off":
                    command.append("--no-rollback")
                if args.limit_presentation_60:
                    command.append("--limit-presentation-60")
                if args.bridge_drag:
                    command.append("--bridge-drag")
                if args.curved_bullets:
                    command.append("--curved-bullets")
                if args.transport == "rtc":
                    command += ["--rtc", "--rtc-input-delay-ms", str(args.one_way_ms),
                                "--rtc-input-jitter-ms", str(args.jitter_ms)]
                if args.real_stage:
                    command += ["--real-stage", str(args.real_stage), "--difficulty", str(args.difficulty)]
                if args.auto_profile:
                    command.append("--auto-profile")
                print(f"START {label}", flush=True)
                started = time.monotonic()
                try:
                    with (output / f"{label}.log").open("w", encoding="utf-8") as log:
                        child = subprocess.Popen(command, cwd=ROOT, stdout=log, stderr=log)
                        try:
                            code = child.wait(timeout=220)
                        except subprocess.TimeoutExpired:
                            # Kill only this test's tree, not other agents' or
                            # the user's browsers. Python-only termination left
                            # orphan browser/GPU processes contaminating later runs.
                            if sys.platform == "win32":
                                subprocess.run(["taskkill", "/PID", str(child.pid), "/T", "/F"],
                                               stdout=log, stderr=log, timeout=15)
                            else:
                                child.kill()
                            child.wait(timeout=15)
                            raise
                    measured = json.loads(raw.read_text(encoding="utf-8")) if raw.is_file() else {"status": "FAIL", "error": "missing result"}
                except subprocess.TimeoutExpired:
                    code, measured = -1, {"status": "FAIL", "error": "process timeout"}
                record = {"label": label, "exit_code": code, "elapsed_seconds": round(time.monotonic() - started, 2),
                          **summarize(measured)}
                if digest(relay) != relay_identity:
                    record.update(status="FAIL", error="relay changed during measurement")
                result["runs"].append(record)
                (output / "summary.json").write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding="utf-8")
                print(json.dumps(record, ensure_ascii=False), flush=True)
                if digest(relay) != relay_identity:
                    return 1
    return 0 if all(run["status"] == "PASS" for run in result["runs"]) else 1


if __name__ == "__main__":
    raise SystemExit(main())
