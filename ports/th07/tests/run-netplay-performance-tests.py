#!/usr/bin/env python3
"""Run pure netplay behavior tests natively and with WASM SAFE_HEAP."""
from __future__ import annotations
import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
COMMON = ROOT / "third_party" / "eagler-common"
COMMON_CORE = str(COMMON / "src/netplay/NetplayCore.cpp")
COMMON_PROTOCOL = str(COMMON / "src/netplay/NetplayProtocol.cpp")
COMMON_JOURNAL = str(COMMON / "src/netplay/RollbackJournal.cpp")
COMMON_INPUT = str(COMMON / "src/netplay/NetplayInput.cpp")
CASES = {
    "dormant-bullet": ["tests/dormant-bullet-snapshot-test.cpp"],
    "partitioned-pool": ["tests/partitioned-pool-journal-test.cpp"],
    "live-bullet-snapshot": ["tests/live-bullet-snapshot-test.cpp"],
    "rollback-replay-budget": ["tests/netplay-rollback-replay-budget-test.cpp"],
    "direct-touch-equivalence": ["tests/direct-touch-equivalence-test.cpp"],
    "compact-bullet-snapshot": ["tests/compact-bullet-snapshot-test.cpp"],
    "item-presentation": ["tests/item-presentation-test.cpp"],
    "bullet-live-prev": ["tests/bullet-live-prev-test.cpp"],
    "coalesced-restore": ["tests/journal-coalesced-restore-test.cpp", COMMON_JOURNAL],
    "stable-draw-order": ["tests/stable-draw-order-test.cpp"],
    "copy-backends": ["tests/netplay-copy-backend-test.cpp", COMMON_JOURNAL],
    "sparse-pool": ["tests/netplay-sparse-pool-test.cpp", COMMON_JOURNAL],
    "confirmed-prefix": ["tests/netplay-confirmed-prefix-test.cpp", COMMON_CORE,
                         COMMON_PROTOCOL, COMMON_JOURNAL],
    "snapshot-policy": ["tests/netplay-snapshot-policy-test.cpp", COMMON_JOURNAL],
    "touch-pipeline": ["tests/netplay-touch-pipeline-test.cpp", COMMON_CORE,
                       COMMON_PROTOCOL, COMMON_INPUT,
                       COMMON_JOURNAL],
    "delay-prediction": ["tests/netplay-delay-prediction-test.cpp", COMMON_CORE,
                         COMMON_PROTOCOL],
    "rollback-journal": ["tests/rollback-journal-test.cpp", COMMON_JOURNAL],
    "frame-budget": ["tests/netplay-frame-budget-test.cpp"],
    "frame-advantage": ["tests/netplay-frame-advantage-test.cpp"],
    "netplay-core": ["tests/netplay-core-test.cpp", COMMON_CORE,
                     COMMON_PROTOCOL, str(COMMON / "src/netplay/NetplaySession.cpp")],
}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--toolchain", choices=("native", "wasm", "both"), default="both")
    parser.add_argument("--output", type=Path)
    parser.add_argument("--cases", nargs="+", choices=tuple(CASES), help="Run only named cases; default remains the complete suite")
    args = parser.parse_args()
    report = {"status": "PASS", "tests": []}
    env = os.environ.copy()
    emsdk = ROOT.parent / "toolchains/emsdk"
    env["EM_CONFIG"] = str(emsdk / ".emscripten")
    empp = str(emsdk / "upstream/emscripten" / ("em++.exe" if os.name == "nt" else "em++"))
    try:
        with tempfile.TemporaryDirectory(prefix="th07-netplay-tests-") as directory:
            for toolchain in (("native", "wasm") if args.toolchain == "both" else (args.toolchain,)):
                for name, sources in CASES.items():
                    if args.cases and name not in args.cases:
                        continue
                    executable = Path(directory) / (name + (".cjs" if toolchain == "wasm" else ".exe"))
                    command = [empp if toolchain == "wasm" else "g++", "-std=c++17", "-O2", "-UNDEBUG",
                               "-Wall", "-Wextra", "-Isrc", "-I", str(COMMON / "include"),
                               *sources, "-o", str(executable)]
                    if name in ("bullet-live-prev", "item-presentation", "compact-bullet-snapshot", "live-bullet-snapshot", "dormant-bullet"):
                        command += ["-Ivendored/SDL/include"]
                    if toolchain == "wasm":
                        # The standalone core test places several complete
                        # input histories on its stack (production uses globals).
                        command += ["-sASSERTIONS=2", "-sSAFE_HEAP=1", "-sSTACK_SIZE=1048576",
                                    "-sALLOW_MEMORY_GROWTH=1", "-sENVIRONMENT=node"]
                    subprocess.run(command, cwd=ROOT, env=env, check=True, timeout=120)
                    run = ["node", str(executable)] if toolchain == "wasm" else [str(executable)]
                    text = subprocess.check_output(run, cwd=ROOT, text=True, timeout=90).strip()
                    report["tests"].append({"toolchain": toolchain, "name": name, "result": text})
                    print(f"{toolchain}/{name}: {text}", flush=True)
    except (subprocess.SubprocessError, OSError) as error:
        report["status"] = "FAIL"
        report["error"] = str(error)
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2), flush=True)
    return 0 if report["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
