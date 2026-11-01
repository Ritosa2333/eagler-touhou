#!/usr/bin/env python3
"""Launch durable, bounded Linux test jobs using the system service manager."""
import argparse
import datetime
import json
import os
from pathlib import Path
import pwd
import signal
import subprocess
import sys
import uuid

SLICE = "touhou-tests.slice"


def now():
    return datetime.datetime.now(datetime.timezone.utc).isoformat()


def atomic_json(path, value):
    temporary = path.with_suffix(".tmp")
    temporary.write_text(json.dumps(value, indent=2) + "\n")
    temporary.replace(path)


def record_result(directory):
    job = Path(directory)
    result = os.environ.get("SERVICE_RESULT", "unknown")
    kind = os.environ.get("EXIT_CODE", "")
    status = os.environ.get("EXIT_STATUS", "")
    if result == "timeout":
        code = 124
    elif result == "oom-kill":
        code = 137
    elif kind == "exited" and status.isdecimal():
        code = int(status)
        if result != "success" and code == 0:
            code = 125
    elif kind in ("killed", "dumped"):
        number = int(status) if status.isdecimal() else getattr(signal, "SIG" + status, 0)
        code = 128 + int(number) if number else 125
    else:
        code = 125
    group = Path("/proc/self/cgroup").read_text().strip().split(":", 2)[2]
    cg = Path("/sys/fs/cgroup" + group)
    events = {name: int(value) for name, value in
              (line.split() for line in (cg / "memory.events").read_text().splitlines())}
    metadata = {
        "finishedAt": now(), "serviceResult": result, "exitStatus": code,
        "systemdExitCode": kind, "systemdExitStatus": status,
        "cgroup": group, "memoryPeakBytes": int((cg / "memory.peak").read_text()),
        "memoryEvents": events,
    }
    (job / "exit").write_text(str(code) + "\n")
    atomic_json(job / "result.json", metadata)


def call(*args):
    return subprocess.run(args, check=True, capture_output=True, text=True).stdout


def exec_property(arguments):
    # Unit command lines use systemd quoting, not a shell. ':' disables $ expansion.
    return ":" + " ".join(json.dumps(str(value).replace("%", "%%"), ensure_ascii=False) for value in arguments)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--owner", required=True, help="Unprivileged account used by the test")
    parser.add_argument("--job-dir", required=True, type=Path)
    parser.add_argument("--cwd", required=True, type=Path)
    parser.add_argument("--timeout", type=int, default=2700, help="Wall-clock seconds, enforced outside the test")
    parser.add_argument("--stop-timeout", type=int, default=5)
    parser.add_argument("--memory-mib", type=int, default=4096)
    parser.add_argument("--env", action="append", default=[], metavar="NAME=VALUE")
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if not 1 <= args.timeout <= 7200:
        parser.error("timeout must be between 1 and 7200 seconds")
    if not 3 <= args.stop_timeout <= 30:
        parser.error("stop-timeout must be between 3 and 30 seconds")
    if not 64 <= args.memory_mib <= 8192:
        parser.error("memory-mib must be between 64 and 8192")
    if sys.platform != "linux" or os.geteuid() != 0:
        parser.error("requires Linux and the root system manager; no unbounded fallback")
    owner = pwd.getpwnam(args.owner)
    if owner.pw_uid == 0:
        parser.error("test owner must be unprivileged")
    command = args.command[1:] if args.command[:1] == ["--"] else args.command
    if not command:
        parser.error("a test command is required after --")
    cwd = args.cwd.resolve(strict=True)
    if not cwd.is_dir():
        parser.error("cwd must be a directory")
    job = args.job_dir.absolute()
    # Never reinterpret an earlier job as a new launch after SSH reconnection.
    if job.exists():
        parser.error("job-dir already exists; inspect its result instead of replaying it")
    environment = {name: os.environ[name] for name in (
        "PATH", "HTTPS_PROXY", "HTTP_PROXY", "https_proxy", "http_proxy", "NO_PROXY", "no_proxy"
    ) if name in os.environ}
    for entry in args.env:
        name, separator, value = entry.partition("=")
        if not separator or not name.isidentifier() or not name.isascii():
            parser.error("env must have an ASCII NAME=VALUE")
        environment[name] = value
    environment.setdefault("GOMAXPROCS", "2")
    environment.update(HOME=owner.pw_dir, USER=args.owner, LOGNAME=args.owner)
    unit = "touhou-test-" + uuid.uuid4().hex[:16] + ".service"
    # The aggregate cap also bounds concurrent jobs. It is separate from user.slice.
    call("systemctl", "start", SLICE)
    call("systemctl", "set-property", "--runtime", SLICE,
         "MemoryAccounting=yes", "MemoryHigh=6G", "MemoryMax=8G",
         "MemorySwapMax=0", "CPUQuota=200%", "TasksMax=1024")
    actual = call("systemctl", "show", SLICE, "-p", "MemoryMax", "-p", "MemorySwapMax")
    if "MemoryMax=8589934592" not in actual or "MemorySwapMax=0" not in actual:
        raise RuntimeError("aggregate resource limits did not take effect")
    job.mkdir(parents=True, mode=0o700)
    os.chown(job, owner.pw_uid, owner.pw_gid)
    atomic_json(job / "job.json", {
        "startedAt": now(), "host": os.uname().nodename, "unit": unit,
        "cwd": str(cwd), "owner": args.owner, "command": command,
        "memoryMiB": args.memory_mib, "timeoutSeconds": args.timeout,
        "environmentNames": sorted(environment),
    })
    properties = {
        # Match the hard cap so one runaway job fails instead of reclaiming forever.
        "MemoryAccounting": "yes", "MemoryHigh": str(args.memory_mib) + "M",
        "MemoryMax": str(args.memory_mib) + "M", "MemorySwapMax": "0",
        "RuntimeMaxSec": str(args.timeout), "TimeoutStartSec": "15",
        "TimeoutStopSec": str(args.stop_timeout), "KillMode": "control-group",
        "OOMPolicy": "kill", "LimitCORE": "0", "TasksMax": "512",
        "Restart": "no", "StandardOutput": "append:" + str(job / "output.log").replace("%", "%%"),
        "StandardError": "inherit",
        "ExecStopPost": exec_property([sys.executable, Path(__file__).resolve(), "--record", job]),
    }
    invocation = ["systemd-run", "--unit=" + unit, "--slice=" + SLICE, "--service-type=exec",
                  "--uid=" + args.owner, "--gid=" + str(owner.pw_gid),
                  "--working-directory=" + str(cwd), "--expand-environment=no", "--no-ask-password"]
    for name, value in properties.items():
        invocation += ["--property=" + name + "=" + value]
    for name, value in environment.items():
        invocation += ["--setenv=" + name + "=" + value]
    try:
        call(*invocation, "--", *command)
    except subprocess.CalledProcessError as error:
        atomic_json(job / "launch-error.json", {"time": now(), "error": error.stderr})
        raise
    print(json.dumps({"unit": unit, "jobDirectory": str(job), "result": str(job / "result.json")}))


if __name__ == "__main__":
    if len(sys.argv) == 3 and sys.argv[1] == "--record":
        record_result(sys.argv[2])
    else:
        main()
