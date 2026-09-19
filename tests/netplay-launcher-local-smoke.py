#!/usr/bin/env python3
"""Exercise real Launcher room modes against a LOCAL generated candidate site.

Copies one explicitly named, bounded existing deployment template. It never
publishes, updates that template, or starts a service on a non-loopback address.
The generated site and logs remain available for diagnosis on failure.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import socket
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[1]
LAUNCHER = ROOT.parent / "eagler-touhou"


def free_port() -> int:
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def stop_owned(process: subprocess.Popen) -> None:
    if process.poll() is not None:
        return
    if os.name == "nt":
        subprocess.run(["taskkill", "/PID", str(process.pid), "/T", "/F"],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=15)
    else:
        process.terminate()
    process.wait(timeout=15)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--template", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--build", default="build-web-th07-netplay")
    parser.add_argument("--players", type=int, choices=(2, 3), default=2)
    parser.add_argument("--mobile-seat", type=int, default=1)
    parser.add_argument("--all-mobile", action="store_true")
    parser.add_argument("--import-package", action="store_true", help="Exercise local package import even on a hosted-resource template")
    args = parser.parse_args()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    site = output / "site"
    template = args.template.resolve()
    if not (template / "deployment.json").is_file():
        parser.error("an existing deployment template is required")
    shutil.copytree(template, site, ignore=shutil.ignore_patterns(".tmp"))
    http_port, relay_port = free_port(), free_port()
    runtime = ROOT / args.build
    version = hashlib.sha256((runtime / "th07.wasm").read_bytes()).hexdigest()[:16]
    relative = Path("runtime/th07/multiplayer")
    for suffix in ("html", "js", "wasm"):
        shutil.copyfile(runtime / f"th07.{suffix}", site / relative / f"th07.{suffix}")

    manifest_path = site / "host-manifest.json"
    host = json.loads(manifest_path.read_text(encoding="utf-8"))
    host["games"]["th07"]["multiplayerRuntime"] = f"{relative.as_posix()}/th07.html?hosted=1&v={version}"
    host["shared"]["netplayRelay"] = f"ws://127.0.0.1:{relay_port}/"
    host["shared"].pop("originMigration", None)
    manifest_path.write_text(json.dumps(host, indent=2, ensure_ascii=False), encoding="utf-8")
    deployment_path = site / "deployment.json"
    deployment = json.loads(deployment_path.read_text(encoding="utf-8"))
    changed = {"host-manifest.json", *(f"{relative.as_posix()}/th07.{s}" for s in ("html", "js", "wasm"))}
    for entry in deployment["files"]:
        if entry["path"] in changed:
            data = (site / entry["path"]).read_bytes()
            entry.update(bytes=len(data), sha256=hashlib.sha256(data).hexdigest())
    deployment_path.write_text(json.dumps(deployment, indent=2, ensure_ascii=False), encoding="utf-8")
    with (output / "frontend-build.log").open("w", encoding="utf-8") as log:
        subprocess.run(["node", "scripts/refresh-deployment-app-shell.mjs", str(site), "--frontend",
                        f"--site-url=http://127.0.0.1:{http_port}/", f"--artwork-dir={site / 'assets'}"],
                       cwd=LAUNCHER, stdout=log, stderr=log, check=True, timeout=90)
        subprocess.run(["node", "scripts/verify-server-build.mjs", str(site)],
                       cwd=LAUNCHER, stdout=log, stderr=log, check=True, timeout=60)

    env = {k:v for k,v in os.environ.items() if not k.startswith(("EAGLER_NETPLAY_", "TH07_"))}
    env.update(EAGLER_NETPLAY_RELAY_HOST="127.0.0.1", EAGLER_NETPLAY_RELAY_PORT=str(relay_port),
               EAGLER_NETPLAY_STUN_URLS="", EAGLER_TOUHOU_HOST="127.0.0.1")
    children = []
    result = {"status":"FAIL", "runtime_version":version, "settings":vars(args)}
    try:
        with (output / "relay.log").open("w", encoding="utf-8") as relay_log, \
             (output / "site.log").open("w", encoding="utf-8") as site_log, \
             (output / "launcher-test.log").open("w", encoding="utf-8") as test_log:
            children.append(subprocess.Popen(["node", "server/netplay-relay.mjs"], cwd=LAUNCHER,
                                             env=env, stdout=relay_log, stderr=relay_log))
            children.append(subprocess.Popen(["node", "scripts/serve.mjs", str(http_port), str(site)],
                                             cwd=LAUNCHER, env=env, stdout=site_log, stderr=site_log))
            for port in (relay_port, http_port):
                deadline = time.monotonic() + 30
                while True:
                    if any(p.poll() is not None for p in children):
                        raise RuntimeError("local test service exited; inspect logs")
                    try:
                        with socket.create_connection(("127.0.0.1", port), timeout=1): break
                    except OSError:
                        if time.monotonic() >= deadline: raise
                        time.sleep(.1)
            command = [sys.executable, "tests/test-th07mp-launch.py", f"http://127.0.0.1:{http_port}/",
                       str(args.players), "--browser-channel=msedge"]
            if args.import_package or host["shared"].get("resourceMode") == "external":
                command.append(f"--package-zip={ROOT / '.codex-tmp/th07-launch-e2e-pack.zip'}")
            if args.all_mobile: command.append("--all-mobile")
            elif args.mobile_seat >= 0: command.append(f"--mobile-seat={args.mobile_seat}")
            test = subprocess.Popen(command, cwd=LAUNCHER, stdout=test_log, stderr=test_log)
            children.append(test)
            code = test.wait(timeout=210)
            if code: raise RuntimeError(f"Launcher test exit {code}")
            result["status"] = "PASS"
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        result["error"] = str(error)
    finally:
        for process in reversed(children): stop_owned(process)
        (output / "result.json").write_text(json.dumps(result, default=str, indent=2), encoding="utf-8")
    print(json.dumps(result, default=str), flush=True)
    print((output / "launcher-test.log").read_text(encoding="utf-8", errors="replace")[-6500:], flush=True)
    return 0 if result["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
