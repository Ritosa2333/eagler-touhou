"""Run an isolated original TH19 copy with the D3D9 observation provider."""
import argparse
import ctypes
from ctypes import wintypes
import hashlib
import json
import msvcrt
import os
from pathlib import Path
import shutil
import subprocess
import struct
import tempfile
import time

EXPECTED_EXE = "804107d9033601f69c251a5e6e449bd88d9add36591129db394765295893bac6"


class BattleProgress:
    def __init__(self, trace):
        self.trace = trace
        self.offset = 0
        self.samples = 0
        self.last_opening_frame = None
        self.opening_rules = None
        self.fast_state_active = False

    def poll(self):
        try:
            stream = self.trace.open("rb")
        except FileNotFoundError:
            return
        with stream:
            stream.seek(self.offset)
            for line in stream:
                if not line.endswith(b"\n"):
                    break
                row = json.loads(line)
                if row.get("fastStateActive"):
                    self.fast_state_active = True
                if row.get("segment") == 0 and "settingsWords" in row:
                    self.opening_rules = row["settingsWords"]
                if "frame" in row:
                    self.samples += 1
                    if row.get("segment") == 0:
                        self.last_opening_frame = row["frame"]
                self.offset += len(line)


def start_cpu_fixture(process, fixture, timeout, pipe_name):
    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    get_owner = kernel.GetNamedPipeServerProcessId
    get_owner.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.ULONG)]
    get_owner.restype = wintypes.BOOL
    until = time.monotonic() + min(15, timeout)
    while time.monotonic() < until and process.poll() is None:
        try:
            fd = os.open(f"\\\\.\\pipe\\{pipe_name}", os.O_WRONLY | os.O_BINARY)
        except OSError:
            time.sleep(.1)
            continue
        with os.fdopen(fd, "wb", buffering=0) as pipe:
            owner = wintypes.ULONG()
            if not get_owner(msvcrt.get_osfhandle(pipe.fileno()), ctypes.byref(owner)):
                raise ctypes.WinError(ctypes.get_last_error())
            if owner.value != process.pid:
                raise RuntimeError("Replay player pipe belongs to a different process")
            path = str(fixture).encode("utf8")
            packet = struct.pack("<I", len(path)) + path
            if pipe.write(packet) != len(packet):
                raise OSError("Incomplete CPU fixture request")
            return
    raise RuntimeError("Isolated CPU driver did not open its pipe")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, required=True)
    parser.add_argument("--observer", type=Path, required=True)
    parser.add_argument("--game-config", type=Path,
                        help="Copy this th19.cfg into the isolated profile")
    parser.add_argument("--score-profile", type=Path,
                        help="Copy an existing unlocked score file into the isolated profile")
    parser.add_argument("--output-dir", type=Path, required=True,
                        help="Capture directory on the original game's NTFS volume")
    parser.add_argument("--timeout", type=int, default=180)
    parser.add_argument("--stop-at-frame", type=int,
                        help="Stop after this simulated frame in the opening round")
    parser.add_argument("--replay", type=Path)
    parser.add_argument("--cpu-fixture", type=Path)
    parser.add_argument("--cpu-start-delay", type=int, default=0)
    parser.add_argument("--cpu-driver", type=Path)
    parser.add_argument("--cpu-driver-sha256")
    parser.add_argument("--isolated-pipe", action="store_true",
                        help="Use a unique pipe with the repository's patched CPU driver")
    parser.add_argument("--cpu-second-rng", type=int)
    parser.add_argument("--rng-calls", action="store_true")
    parser.add_argument("--bullet-positions", action="store_true", help="sample live enemy bullet coordinates in both fields")
    parser.add_argument("--fast-state", action="store_true", help="run CPU battle capture without a frame-rate cap, for state comparisons only")
    parser.add_argument("--minimized", action="store_true", help="start the isolated game minimized without taking focus")
    parser.add_argument("--trace-window", nargs=2, type=int, metavar=("FIRST", "LAST"),
                        help="Limit RNG and collision detail to this inclusive frame range")
    args = parser.parse_args()
    if args.timeout < 1:
        parser.error("timeout must be positive")
    if args.stop_at_frame is not None and args.stop_at_frame < 1:
        parser.error("stop-at-frame must be positive")
    if not 0 <= args.cpu_start_delay < args.timeout:
        parser.error("CPU start delay must be smaller than the timeout")
    if args.cpu_fixture and args.replay:
        parser.error("Choose one fixture format")
    if args.cpu_fixture and not args.cpu_driver:
        parser.error("A CPU fixture requires the local CPU driver")
    if bool(args.cpu_driver) != bool(args.cpu_driver_sha256):
        parser.error("Supply both the CPU driver path and SHA-256")
    if args.cpu_driver and not args.cpu_fixture:
        parser.error("A CPU driver requires a CPU fixture")
    if args.isolated_pipe and not args.cpu_driver:
        parser.error("isolated-pipe requires the local CPU driver")
    if args.cpu_second_rng is not None and not args.cpu_driver:
        parser.error("The second RNG state requires the local CPU driver")
    if args.rng_calls and not args.cpu_fixture:
        parser.error("RNG call tracing requires an isolated CPU fixture")
    if args.bullet_positions and not args.rng_calls:
        parser.error("bullet-positions requires rng-calls")
    if args.fast_state and not args.cpu_fixture:
        parser.error("fast-state requires a CPU fixture")
    if args.trace_window and (not args.rng_calls or not 0 <= args.trace_window[0] <= args.trace_window[1] <= 36000):
        parser.error("trace-window requires rng-calls and a valid ordered frame range")
    if args.cpu_second_rng is not None and not 0 < args.cpu_second_rng < 2147483647:
        parser.error("Invalid second RNG state")

    source = args.original.resolve()
    output = args.output_dir.resolve()
    output.mkdir(parents=True, exist_ok=True)
    original = (source / "th19.exe").read_bytes()
    if hashlib.sha256(original).hexdigest() != EXPECTED_EXE:
        raise ValueError("Unexpected original executable SHA-256")
    work = Path(tempfile.mkdtemp(prefix="present-", dir=output))
    old = "th19 App\0".encode("utf-16le")
    if original.count(old) != 1:
        raise ValueError("Unexpected original mutex identity")
    unique = (work.name[-4:] + " App\0").encode("utf-16le")
    copied = original.replace(old, unique, 1)
    (work / "th19.exe").write_bytes(copied)
    for name in ("th19.dat", "thbgm.dat"):
        os.link(source / name, work / name)
    shutil.copy2(args.observer, work / "d3d9.dll")

    real_profile = Path(os.environ["APPDATA"]) / "ShanghaiAlice" / "th19"
    appdata = work / "appdata"
    profile = appdata / "ShanghaiAlice" / "th19"
    (profile / "replay").mkdir(parents=True)
    config = args.game_config or real_profile / "th19.cfg"
    config_hash = None
    if config.is_file():
        test_config = profile / "th19.cfg"
        shutil.copy2(config, test_config)
        config_hash = hashlib.sha256(test_config.read_bytes()).hexdigest()
    elif args.game_config:
        raise FileNotFoundError(config)
    if args.score_profile:
        shutil.copy2(args.score_profile, profile / "scoreth19.dat")
    replay_hash = None
    driver_hash = None
    if args.replay:
        replay = args.replay.read_bytes()
        if replay[:4] != b"t18r":
            raise ValueError("Expected an original TH19 replay container")
        replay_hash = hashlib.sha256(replay).hexdigest()
        (profile / "replay" / "th19_01.rpy").write_bytes(replay)
    fixture = None
    if args.cpu_fixture:
        replay = args.cpu_fixture.read_bytes()
        if len(replay) < 21 or replay[9] != 2 or (len(replay) - 17) % 4:
            raise ValueError("Expected a Ju.N.Owen CPU-vs-CPU fixture")
        fixture = work / "cpu-fixture.bin"
        fixture.write_bytes(replay)
        replay_hash = hashlib.sha256(replay).hexdigest()
        driver = args.cpu_driver
        driver_hash = hashlib.sha256(driver.read_bytes()).hexdigest()
        if driver_hash != args.cpu_driver_sha256.lower():
            raise ValueError("Unexpected CPU driver SHA-256")
        shutil.copy2(driver, work / "th19replayplayer_hook.dll")
    summary = {
        "originalExecutableSha256": EXPECTED_EXE,
        "testExecutableSha256": hashlib.sha256(copied).hexdigest(),
        "testExecutableChange": "Private Win32 singleton mutex string",
        "testConfigSha256": config_hash,
        "replayFixtureSha256": replay_hash,
        "boundary": "D3D9 pre-Present",
        "replayPlaybackVerified": False,
        "cpuFixtureSubmitted": False,
        "cpuDriverSha256": driver_hash,
        "cpuSecondRng": args.cpu_second_rng,
        "bulletPositions": args.bullet_positions,
    }
    (work / "capture.json").write_text(json.dumps(summary, indent=2) + "\n", encoding="utf8")
    env = dict(os.environ, APPDATA=str(appdata), LOCALAPPDATA=str(appdata))
    pipe_name = "th19replayplayer"
    if args.isolated_pipe:
        pipe_name += f"-{work.name}"
        env["TH19_CAPTURE_PIPE"] = pipe_name
    if args.cpu_driver:
        env["TH19_CAPTURE_CPU"] = "1"
    if args.cpu_second_rng is not None:
        env["TH19_CAPTURE_SECOND_RNG"] = str(args.cpu_second_rng)
    if args.rng_calls:
        env["TH19_CAPTURE_RNG_CALLS"] = "1"
    if args.bullet_positions:
        env["TH19_CAPTURE_BULLET_POSITIONS"] = "1"
    if args.fast_state:
        env["TH19_CAPTURE_FAST_STATE"] = "1"
    if args.trace_window:
        env["TH19_CAPTURE_TRACE_FIRST"], env["TH19_CAPTURE_TRACE_LAST"] = map(str, args.trace_window)
    startup = None
    if args.minimized:
        startup = subprocess.STARTUPINFO()
        startup.dwFlags = subprocess.STARTF_USESHOWWINDOW
        startup.wShowWindow = 7  # SW_SHOWMINNOACTIVE
    process = subprocess.Popen([str(work / "th19.exe")], cwd=work, env=env, startupinfo=startup)
    progress = BattleProgress(work / "native-present.jsonl")
    print(f"CAPTURE={work}", flush=True)
    print(f"PID={process.pid}", flush=True)
    try:
        stop = time.monotonic() + args.timeout
        if fixture:
            time.sleep(args.cpu_start_delay)
            start_cpu_fixture(process, fixture, args.timeout - args.cpu_start_delay, pipe_name)
            summary["cpuFixtureSubmitted"] = True
            (work / "capture.json").write_text(json.dumps(summary, indent=2) + "\n", encoding="utf8")
            print("CPU_FIXTURE_SUBMITTED", flush=True)
        while time.monotonic() < stop and process.poll() is None:
            if args.stop_at_frame is not None:
                progress.poll()
                frame = progress.last_opening_frame
                if frame is not None and frame >= args.stop_at_frame:
                    break
            time.sleep(.2)
    finally:
        summary["exitedBeforeDeadline"] = process.poll() is not None
        if process.poll() is None:
            process.terminate()
        summary["processExitCode"] = process.wait(timeout=10)
        progress.poll()
        summary.update(battlePresentSamples=progress.samples,
                       lastOpeningFrame=progress.last_opening_frame, rules=progress.opening_rules)
        if args.fast_state:
            summary["fastStateActive"] = progress.fast_state_active
        selection = work / "cpu-selection.json"
        if selection.is_file():
            summary["cpuSelection"] = json.loads(selection.read_text(encoding="utf8"))
        (work / "capture.json").write_text(json.dumps(summary, indent=2) + "\n", encoding="utf8")
    if fixture and not summary["battlePresentSamples"]:
        raise RuntimeError(f"CPU fixture produced no battle frames in {work}")
    if args.fast_state and not progress.fast_state_active:
        raise RuntimeError(f"Fast state capture did not activate in {work}")
    if args.stop_at_frame is not None:
        frame = summary["lastOpeningFrame"]
        if frame is None or frame < args.stop_at_frame:
            raise RuntimeError(f"Capture stopped before opening frame {args.stop_at_frame} in {work}")
    if args.cpu_driver:
        expected = {"mode": 2, "matchup": 2, "difficulty": replay[8], "characters": [replay[13], replay[15]]}
        if replay[10] >> 6 == 3:
            expected["cards"] = [replay[14], replay[16]]
        selected = summary.get("cpuSelection", {})
        if any(selected.get(key) != value for key, value in expected.items()):
            raise RuntimeError(f"CPU fixture selection was not verified in {work}")
        if summary["rules"] != list(replay[10:13]):
            raise RuntimeError(f"CPU fixture rules were not verified in {work}")
    if args.rng_calls and not (work / "native-rng-calls.jsonl").stat().st_size:
        raise RuntimeError(f"No original RNG calls captured in {work}")


if __name__ == "__main__":
    main()
