from __future__ import annotations

"""Real th06/th07 native Replay spectator restore and playback controls.

Run from the repository root after building the web targets. The browser test
requires Python Playwright and the local relay's Node dependencies.

    $env:EAGLER_WORKSPACE = (Get-Location).Path
    $env:TH_EAGLER_TH06_ROOT = Join-Path $env:EAGLER_WORKSPACE 'ports/th06'
    $env:TH_EAGLER_TH07_ROOT = Join-Path $env:EAGLER_WORKSPACE 'ports/th07'
    $env:EAGLER_RELAY_ROOT = Join-Path $env:EAGLER_WORKSPACE 'services/relay'
    python -u ports/th07/tests/netplay-spectator-stage2-browser.py th06 th07 --force-relay
    python -u ports/th07/tests/netplay-spectator-stage2-browser.py th06 th07 --players=3

Stage 2 late joining verifies native Replay initialization and the following
600 ticks, automatic segment following, pause, cross-stage seek and live input.
"""

import hashlib
import json
import os
import re
import socket
import subprocess
import sys
import time
import urllib.parse
import urllib.request
from pathlib import Path

from integration_support import require_host_relay, require_th06_root
from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[1]
def find_workspace_root() -> Path:
    explicit = os.environ.get("EAGLER_WORKSPACE")
    if explicit:
        candidate = Path(explicit).expanduser().resolve()
        if not (candidate / "WORKSPACE.md").is_file():
            raise RuntimeError(f"EAGLER_WORKSPACE has no WORKSPACE.md: {candidate}")
        return candidate
    for candidate in (ROOT, *ROOT.parents):
        if (candidate / "WORKSPACE.md").is_file():
            return candidate
    raise RuntimeError("Could not find WORKSPACE.md above the TH07 test repository")


WORKSPACE_ROOT = find_workspace_root()
if not os.environ.get("TH_EAGLER_HOST_ROOT"):
    host = os.environ.get("EAGLER_HOST_ROOT") or os.environ.get("HOST_ROOT")
    os.environ["TH_EAGLER_HOST_ROOT"] = str(
        Path(host).expanduser().resolve() if host else
        WORKSPACE_ROOT / "worktrees" / "launcher-th09-spectator-main"
    )
if not os.environ.get("TH_EAGLER_TH06_ROOT"):
    os.environ["TH_EAGLER_TH06_ROOT"] = str(WORKSPACE_ROOT / "th06-eagler")
if not os.environ.get("TH_EAGLER_TH07_ROOT"):
    os.environ["TH_EAGLER_TH07_ROOT"] = str(ROOT)

HOST_ROOT, RELAY_SCRIPT = require_host_relay()


def free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(("127.0.0.1", 0))
        return int(sock.getsockname()[1])


def wait_http(url: str) -> None:
    deadline = time.time() + 10
    while time.time() < deadline:
        try:
            with urllib.request.urlopen(url, timeout=0.5) as response:
                if response.status < 400:
                    return
        except Exception:
            time.sleep(0.1)
    raise RuntimeError(f"HTTP server did not start: {url}")


def wait_relay(process: subprocess.Popen[str]) -> None:
    assert process.stdout is not None
    deadline = time.time() + 10
    while time.time() < deadline:
        line = process.stdout.readline()
        if " relay listening " in line.lower():
            return
        if process.poll() is not None:
            raise RuntimeError(f"relay exited early: {process.returncode}")
    raise RuntimeError("relay did not start")


def admit_lobby(setup, relay_base: str, room: str, spectator_id: str,
                player_count: int, difficulty: int = 1, challenge: bool = False) -> dict:
    return setup.evaluate(
        """async ({ relayBase, room, spectatorId, playerCount, difficulty, challenge }) => {
          function connect(id, member, intent) {
            return new Promise((resolve, reject) => {
              const ws = new WebSocket(`${relayBase}/?room=${room}&lobby=${id}&member=${member}&intent=${intent}&players=${playerCount}&difficulty=${difficulty}&challengeMode=${challenge ? 1 : 0}`);
              const queue = [];
              const waiters = [];
              ws.onmessage = event => {
                const value = JSON.parse(String(event.data));
                const waiter = waiters.shift();
                if (waiter) waiter(value); else queue.push(value);
              };
              ws.onerror = () => reject(new Error(`lobby socket failed: ${id}`));
              ws.onopen = () => resolve({
                ws,
                next() {
                  if (queue.length) return Promise.resolve(queue.shift());
                  return new Promise((resolveNext, rejectNext) => {
                    const timer = setTimeout(() => rejectNext(new Error(`lobby response timeout: ${id}`)), 10000);
                    waiters.push(value => { clearTimeout(timer); resolveNext(value); });
                  });
                },
              });
            });
          }
          async function nextType(client, type) {
            for (;;) {
              const value = await client.next();
              if (value.type === type) return value;
            }
          }
          async function nextReadyRoom(client, requireReady = true) {
            for (;;) {
              const value = await client.next();
              const seats = value.room?.seats || [];
              const active = seats.slice(0, Number(value.room?.playerCount || 0));
              if (value.type === 'error') throw new Error(value.error || 'lobby rejected test setup');
              if (value.type === 'state' && active.length === Number(playerCount) &&
                  Number(value.room?.spectatorCount) === 1 &&
                  active.every(seat => seat && (!requireReady || seat.ready === true))) return value;
            }
          }
          const roster = [
            { id: 'player_one_0001', member: 'member_p1_000001', loadout: 0 },
            { id: 'player_two_0002', member: 'member_p2_000002', loadout: 1 },
            { id: 'player_three_0003', member: 'member_p3_000003', loadout: 2 },
          ].slice(0, Number(playerCount));
          const players = [];
          for (let index = 0; index < roster.length; ++index) {
            const player = await connect(roster[index].id, roster[index].member,
              index === 0 ? 'create' : 'join');
            await player.next();
            players.push(player);
          }
          const spectator = await connect(spectatorId, 'member_view_0003', 'join'); await spectator.next();
          for (let index = 0; index < players.length; ++index)
            players[index].ws.send(JSON.stringify({ type: 'take-seat', seat: index,
              loadout: roster[index].loadout }));
          spectator.ws.send(JSON.stringify({ type: 'spectate' }));
          // Seat/loadout changes invalidate all ready votes. Wait for the
          // final roster before explicitly acknowledging its settings version.
          await nextReadyRoom(spectator, false);
          for (const player of players)
            player.ws.send(JSON.stringify({ type: 'set-ready', ready: true }));
          await nextReadyRoom(spectator);
          const started = nextType(spectator, 'start');
          players[0].ws.send(JSON.stringify({ type: 'start' }));
          const snapshot = await started;
          globalThis.__spectatorLobbySockets = [...players.map(player => player.ws), spectator.ws];
          return { serial: snapshot.serial, spectatorCount: snapshot.room?.spectatorCount,
            playerCount: snapshot.room?.playerCount };
        }""",
        {"relayBase": relay_base, "room": room, "spectatorId": spectator_id,
         "playerCount": player_count, "difficulty": difficulty, "challenge": challenge},
    )


def snapshot(page, frame: int) -> dict:
    value = page.evaluate(
        """async frame => {
          const artifact = await (globalThis.__spectatorRuntimeArtifact || Promise.resolve(null));
          const runtime = document.getElementById('runtime')?.contentWindow;
          const transport = runtime?.__th06PeerTransport || runtime?.__th07PeerTransport;
          const native = runtime?.Module;
          let nativePlayback = null;
          const readPlayback = native?._th06_mp_spectator_state || native?._th07_mp_spectator_state;
          if (readPlayback) {
            const pointer = readPlayback();
            if (pointer) {
              const heap = native.HEAPU8 || runtime.HEAPU8 || runtime.eval('HEAPU8'); let end = pointer;
              while (end < heap.length && heap[end]) ++end;
              nativePlayback = JSON.parse(new TextDecoder().decode(heap.subarray(pointer, end)));
            }
          }
          return {
            servedRuntimeBuild: String(artifact?.runtimeBuild || ''),
            servedWasmUrl: String(artifact?.wasmUrl || ''),
            servedWasmSha256: String(artifact?.wasmSha256 || ''),
            servedWasmError: String(artifact?.error || ''),
            runtimePath: String(runtime?.location?.pathname || ''),
            renderer: String(globalThis.__runtimeRenderer || ''),
            launched: !!globalThis.__spectatorSmokeLaunched,
            hostFailure: String(globalThis.__spectatorSmokeFailure || ''),
            active: !!runtime?.__eaglerNetplayLanActive,
            spectator: !!runtime?.__eaglerNetplaySpectator,
            playerCount: Number(native?.eaglerOptions?.netplayPlayerCount || transport?.playerCount || 2),
            frame: Number(runtime?.__eaglerNetplayLanFrame || 0),
            confirmed: Number(runtime?.__eaglerNetplayLanConfirmed ?? -1),
            highestStage: Number(runtime?.__eaglerNetplayLanHighestStage || 0),
            stageTransition: !!runtime?.__eaglerNetplayLanStageTransitionObserved,
            stageTelemetryReady: !!runtime &&
              Object.prototype.hasOwnProperty.call(runtime, '__eaglerNetplayLanHighestStage') &&
              Object.prototype.hasOwnProperty.call(runtime, '__eaglerNetplayLanStageTransitionObserved'),
            checkpointFrame: Number(runtime?.__eaglerNetplayLanCheckpointFrame ?? -1),
            checkpointHash: String(runtime?.__eaglerNetplayLanCheckpointHash || ''),
            checkpointStage: Number(runtime?.__eaglerNetplayLanCheckpointStage ?? -1),
            checkpointConfirmed: Number(runtime?.__eaglerNetplayLanCheckpointConfirmed ?? -1),
            checkpointPredictedMask: Number(runtime?.__eaglerNetplayLanCheckpointPredictedMask ?? -1),
            checkpointCaptureFrame: Number(runtime?.__eaglerNetplayLanCheckpointCaptureFrame ?? -1),
            checkpointCaptureHash: String(runtime?.__eaglerNetplayLanCheckpointCaptureHash || ''),
            checkpointCaptureConfirmed: Number(runtime?.__eaglerNetplayLanCheckpointCaptureConfirmed ?? -1),
            checkpointCapturePredictedMask: Number(runtime?.__eaglerNetplayLanCheckpointCapturePredictedMask ?? -1),
            rollback: Number(runtime?.__eaglerNetplayLanRollback || 0),
            hash: String(runtime?.__eaglerNetplayLanHashes?.[String(frame)] || ''),
            canonical: runtime?.__eaglerNetplayLanCanonical?.[String(frame)] || null,
            replayResources: runtime?.__eaglerNetplayLanReplayResources?.[String(frame)] || null,
            canonicalMeta: runtime?.__eaglerNetplayLanCanonicalMeta?.[String(frame)] || runtime?.__eaglerNetplayLanCanonicalDetail?.[String(frame)] || null,
            route: String(transport?.route || ''),
            spectatorCount: Number(transport?.spectatorCount ?? -1),
            spectatorTransport: transport ? {
              route: String(transport.route || ''),
              stopped: !!transport.spectatorStopped,
              spectatorCount: Number(transport.spectatorCount ?? -1),
              closed: !!transport.closed,
              failed: !!transport.failed,
              error: String(transport.error || ''),
              relayReadyState: Number(transport.relay?.readyState ?? -1),
              relayBufferedAmount: Number(transport.relay?.bufferedAmount ?? -1),
              playerCount: Number(transport.playerCount ?? -1),
              localPlayer: Number(transport.localPlayer ?? -1),
            } : null,
            received: Number(transport?.received?.length || 0) - Number(transport?.receivedHead || 0),
            peerCount: Number(transport?.peers?.size || 0),
            canSend: typeof transport?.send === 'function' || typeof transport?.sendTo === 'function',
            failed: !!transport?.failed || !!runtime?.__eaglerNetplayFailed,
            error: String(transport?.error || runtime?.__eaglerNetplayError || ''),
            publish: runtime?.__eaglerNetplaySpectatorPublish || null,
            publishFailed: Number(runtime?.__eaglerNetplaySpectatorPublishFailed ?? -1),
            playback: nativePlayback || globalThis.__spectatorPlaybackReport || null,
            spectatorMenu: !!runtime?.__eaglerSpectatorMenu,
            nativePlayers: runtime?.__spectatorLabPlayers || runtime?.__eaglerNetplayPlayerStates || null,
            teamWipe: !!runtime?.__eaglerNetplayTeamWipeObserved,
            teamWipeRetry: !!runtime?.__eaglerNetplayTeamWipeRetryObserved,
            teamWipeStartFrame: Number(runtime?.__eaglerNetplayTeamWipeStartFrame ?? -1),
            teamWipeRetryFrame: Number(runtime?.__eaglerNetplayTeamWipeRetryFrame ?? -1),
            retryContinue: !!runtime?.__eaglerNetplayRetryContinueObserved,
            retryResourcesReset: !!runtime?.__eaglerNetplayRetryResourcesResetObserved,
            retryContinueFrame: Number(runtime?.__eaglerNetplayRetryContinueFrame ?? -1),
          };
        }""",
        frame,
    )

    # Native Replay stage jumps rebuild UI totals and fade owners. Keep the
    # complete world hash as diagnostic evidence and compare actual resources,
    # RNG and all player/enemy/projectile/item gameplay owners independently.
    canonical, meta = value.get("canonical"), value.get("canonicalMeta")
    if canonical and meta and value.get("replayResources"):
        value["worldHash"] = value["hash"]
        count = value["playerCount"]
        evidence = [meta[0], value["replayResources"][:4 + count * 3]] + [canonical.get(k) for k in
            ("multiplayer", "stage", "player", "enemies", "bullets", "items", "counts")]
        value["hash"] = hashlib.sha256(json.dumps(evidence, sort_keys=True).encode()).hexdigest()
        value["hashScope"] = "native-replay-gameplay"
    else:
        value["hash"] = ""
    return value


def assert_served_runtime(values: list[dict], game: str, runtime_build: str,
                          expected_wasm_sha256: str, endpoint: str) -> None:
    expected_wasm_url = f"/{runtime_build}/{game}.wasm"
    expected_runtime_path = f"/{runtime_build}/{game}.html"
    mismatches = [value for value in values if (
        value.get("servedRuntimeBuild") != runtime_build or
        value.get("servedWasmUrl") != expected_wasm_url or
        value.get("servedWasmSha256") != expected_wasm_sha256 or
        value.get("servedWasmError") or
        value.get("runtimePath") != expected_runtime_path
    )]
    if mismatches:
        raise AssertionError(
            f"{game.upper()} {endpoint} endpoint loaded the wrong runtime artifact: "
            f"expected build={runtime_build} wasm={expected_wasm_url} "
            f"sha256={expected_wasm_sha256}; endpoints={values}")


def browser_console(message, index: int) -> None:
    text_value = message.text
    if any(marker in text_value.lower() for marker in (
            "netplay", "spectator", "gles renderer", "webgl: invalid_operation",
            "webgl: too many errors", "too many active webgl contexts", "runtime error")):
        print(f"BROWSER[{index}] {text_value}", flush=True)


def admit_late_spectator(setup, relay_base: str, room: str, spectator_id: str,
                         player_count: int) -> dict:
    return setup.evaluate(
        """async ({relayBase,room,spectatorId,playerCount}) => {
          const clientId=spectatorId;
          const memberId='member_view_0004';
          const ws=new WebSocket(`${relayBase}/?room=${room}&lobby=${clientId}&member=${memberId}&intent=join&players=${playerCount}`);
          await new Promise((resolve,reject)=>{ws.onopen=resolve;ws.onerror=()=>reject(new Error('late spectator lobby connect failed'));setTimeout(()=>reject(new Error('late lobby connect timeout')),10000);});
          const start=new Promise((resolve,reject)=>{
            const timer=setTimeout(()=>reject(new Error('late spectator was not admitted')),10000);
            ws.onmessage=event=>{const value=JSON.parse(String(event.data));if(value.type==='error'){clearTimeout(timer);reject(new Error(value.error||'late spectator rejected'));}
              if(value.type==='spectator-start'){clearTimeout(timer);resolve({serial:value.serial,spectatorCount:value.room?.spectatorCount});}};
          });
          ws.send(JSON.stringify({type:'spectate'}));
          globalThis.__lateSpectatorLobby=ws;
          return await start;
        }""",
        {"relayBase": relay_base, "room": room, "spectatorId": spectator_id,
         "playerCount": player_count},
    )


def spectator_command(page, game: str, run: str, action: str,
                      frame: int | None = None) -> dict:
    return page.evaluate(
        """async ({game,run,action,frame}) => {
          const iframe=document.getElementById('runtime');
          const request=`spectator-stage2-${Date.now()}-${Math.random()}`;
          const message={protocol:'eagler-touhou/1',game,epoch:1,
            command:'spectator-playback',request,run,action};
          if(frame!==null) message.frame=frame;
          return await new Promise((resolve,reject)=>{
            const timer=setTimeout(()=>{cleanup();reject(new Error('control timeout'));},5000);
            const listener=event=>{
              if(event.source!==iframe.contentWindow||event.data?.request!==request)return;
              cleanup(); if(event.data.ok)resolve(event.data);
              else reject(new Error(event.data.error||'control rejected'));
            };
            const cleanup=()=>{clearTimeout(timer);removeEventListener('message',listener);};
            addEventListener('message',listener);
            iframe.contentWindow.postMessage(message,location.origin);
          });
        }""",
        {"game": game, "run": run, "action": action, "frame": frame},
    )


def wait_playback(page, predicate, timeout: float = 20.0) -> dict:
    deadline = time.time() + timeout
    state = None
    while time.time() < deadline:
        state = snapshot(page, 300).get("playback")
        if state and predicate(state):
            return state
        time.sleep(0.05)
    raise AssertionError(f"spectator playback state timeout: {state}")


def wait_live_window(game, source_pages, viewer, baseline_position: int,
                     minimum_source_advance: int = 120, timeout: float = 300.0) -> dict:
    initial_sources = [snapshot(page, 300) for page in source_pages]
    source_floor = max(value["frame"] for value in initial_sources)
    required_frame = source_floor + minimum_source_advance
    target_hash_frame = (required_frame // 20) * 20
    deadline = time.time() + timeout
    next_status = time.time()
    max_lag = 0
    fixed_target = (snapshot(viewer, target_hash_frame).get('playback') or {}).get('target')
    latest = None
    while time.time() < deadline:
        sources = [snapshot(page, target_hash_frame) for page in source_pages]
        latest = snapshot(viewer, target_hash_frame)
        state = latest.get("playback") or {}
        if any(value["failed"] or value["hostFailure"] for value in sources + [latest]):
            raise AssertionError(f"{game.upper()} live-window runtime failed: {sources}/{latest}")
        edge = int(state.get("edge", 0))
        position = int(state.get("position", 0))
        max_lag = max(max_lag, max(0, edge - position))
        # Normal playback may retain receive lag. New inputs must not restart
        # the completed Live seek or change its latched destination.
        if state.get('seeking') or not state.get('following') or state.get('target') != fixed_target:
            raise AssertionError(f"{game.upper()} passive playback restarted its Live seek: {state}")
        if time.time() >= next_status:
            received = int(state.get("received", 0))
            print(f"{game.upper()} live-window telemetry: sourceStart={source_floor} "
                  f"required={required_frame} sources={[value['frame'] for value in sources]} "
                  f"position={position} edge={edge} received={received} "
                  f"receivedMinusPosition={received-position} "
                  f"following={state.get('following')} seeking={state.get('seeking')} "
                  f"maxLag={max_lag}", flush=True)
            next_status = time.time() + 5
        if min(value["frame"] for value in sources) >= required_frame and \
                position >= required_frame and state.get("following") is True and \
                not state.get("seeking"):
            compared = sources + [latest]
            hashes = [value["hash"] for value in compared]
            if not all(hashes) or len(set(hashes)) != 1:
                raise AssertionError(f"{game.upper()} 120-source-frame live hash mismatch at "
                                     f"{target_hash_frame}: {compared}; playback={state}")
            return {
                "sourceStart": source_floor,
                "requiredFrame": required_frame,
                "hashFrame": target_hash_frame,
                "hash": hashes[0],
                "viewerPosition": position,
                "edge": edge,
                "received": int(state.get("received", 0)),
                "receivedMinusPosition": int(state.get("received", 0)) - position,
                "maxLag": max_lag,
                "baselinePosition": baseline_position,
            }
        time.sleep(0.1)
    raise AssertionError(f"{game.upper()} viewer failed to stay live for {minimum_source_advance} source frames: "
                         f"{latest}")


def run_game(game: str, force_relay: bool, spectator_delay_ms: int = 0,
             retry_continue: bool = False, browser_channel: str | None = None,
             player_count: int = 2) -> None:
    root = Path(os.environ.get("TH_EAGLER_TH07_ROOT", ROOT)).resolve() if game == "th07" else require_th06_root()
    runtime_build = "build-web-th07-netplay-replay-audit"
    runtime_wasm_sha256 = ""
    if game == "th06":
        runtime_build = os.environ.get("TH06_SPECTATOR_RUNTIME_BUILD", "build-web-netplay-th06")
        parts = runtime_build.split("/")
        if (not parts or runtime_build.startswith("/") or
                any(not part or part in {".", ".."} or not re.fullmatch(r"[A-Za-z0-9._-]+", part)
                    for part in parts)):
            raise ValueError("TH06_SPECTATOR_RUNTIME_BUILD must be a safe relative build directory")
        runtime_dir = root.joinpath(*parts)
        package_files = [runtime_dir / f"th06.{suffix}" for suffix in ("html", "wasm", "js", "data")]
        required_files = package_files[:3]
        missing = [str(path) for path in required_files if not path.is_file()]
        if missing:
            raise FileNotFoundError("TH06 spectator runtime package is incomplete: " + ", ".join(missing))
        package_identity = " ".join(
            f"{path.name}={hashlib.sha256(path.read_bytes()).hexdigest()}"
            for path in package_files if path.is_file())
        runtime_wasm_sha256 = hashlib.sha256(package_files[1].read_bytes()).hexdigest()
        print(f"TH06 spectator runtime package: build={runtime_build} {package_identity}", flush=True)
    else:
        runtime_dir = root / runtime_build
        package_files = [runtime_dir / f"th07.{suffix}" for suffix in ("html", "wasm", "js", "data")]
        missing = [str(path) for path in package_files[:3] if not path.is_file()]
        if missing:
            raise FileNotFoundError("TH07 spectator runtime package is incomplete: " + ", ".join(missing))
        package_identity = " ".join(
            f"{path.name}={hashlib.sha256(path.read_bytes()).hexdigest()}"
            for path in package_files if path.is_file())
        print(f"TH07 spectator runtime package: build={runtime_build} {package_identity}", flush=True)
    runtime_wasm_sha256 = hashlib.sha256(package_files[1].read_bytes()).hexdigest()
    http_port = free_port()
    relay_port = free_port()
    room = f"{game}mp-spectator-{int(time.time() * 1000)}"
    debug_fast_restore = "--debug-fast-stage2-restore" in sys.argv[1:]
    spectator_id = "spectator_0003"
    env = os.environ.copy()
    env.update({
        "TH07_RELAY_HOST": "127.0.0.1",
        "TH07_RELAY_PORT": str(relay_port),
        "TH07_STUN_URLS": "",
        "TH07_RTC_TIMEOUT_MS": "1000" if force_relay else "4500",
    })
    http = subprocess.Popen(
        [sys.executable, "-m", "http.server", str(http_port), "--bind", "127.0.0.1"],
        cwd=root, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    relay = subprocess.Popen(
        ["node", str(RELAY_SCRIPT)], cwd=HOST_ROOT, env=env,
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1,
    )
    browsers = []
    try:
        # Serve the checked-in test host rather than stale copied HTML from a
        # prior build. `stage=1` enables the real native transition fixture on
        # every source player.
        host_path = "tests/netplay-spectator-browser-host.html"
        host = f"http://127.0.0.1:{http_port}/{host_path}"
        wait_http(host)
        wait_relay(relay)
        with sync_playwright() as pw:
            browser = pw.chromium.launch(headless=True, channel=browser_channel, args=[
                "--autoplay-policy=no-user-gesture-required",
                "--disable-background-timer-throttling",
                "--disable-backgrounding-occluded-windows",
                "--disable-renderer-backgrounding",
                "--use-gl=angle",
                "--use-angle=d3d11",
                "--enable-gpu",
                "--ignore-gpu-blocklist",
            ])
            browsers.append(browser)
            setup = browser.new_context().new_page()
            setup.goto(f"http://127.0.0.1:{http_port}/", wait_until="load")
            started = admit_lobby(setup, f"ws://127.0.0.1:{relay_port}", room,
                                  spectator_id, player_count)
            if (started.get("serial") != 1 or started.get("spectatorCount") != 1 or
                    started.get("playerCount") != player_count):
                raise AssertionError(f"bad lobby start: {started}")

            pages = []
            failures = [""] * (player_count + 1)
            member_ids = ["member_p1_000001", "member_p2_000002", "member_p3_000003"]
            for index in range(player_count + 1):
                context = browser.new_context()
                if force_relay:
                    context.add_init_script("delete globalThis.RTCPeerConnection")
                page = context.new_page()
                page.add_init_script("""(() => {
                  globalThis.__spectatorPlaybackReport = null;
                  addEventListener('message', event => {const m=event.data||{};
                    if(m.protocol==='eagler-touhou/1'&&m.game===EXPECTED_GAME&&m.epoch===1&&
                       m.event==='runtime-info') {
                      if (m.renderer) globalThis.__runtimeRenderer=String(m.renderer);
                      if (m.spectatorPlayback?.version===1)
                        globalThis.__spectatorPlaybackReport=m.spectatorPlayback;
                    }});
                })();""".replace("EXPECTED_GAME", repr(game)))
                page.on("pageerror", lambda error, i=index: failures.__setitem__(i, str(error)))
                page.on("console", lambda msg, i=index: browser_console(msg, i))
                pages.append(page)

            relay_query = f"ws://127.0.0.1:{relay_port}/?room={room}&run=1"
            for index, page in enumerate(pages):
                is_initial_viewer = index == player_count
                if is_initial_viewer and spectator_delay_ms > 0:
                    time.sleep(spectator_delay_ms / 1000.0)
                query = {
                    "player": index if not is_initial_viewer else 0,
                    "players": player_count,
                    "relay": relay_query + ("&member=" + (member_ids[index] if not is_initial_viewer else "member_view_0003")) + ("&spectator=" + spectator_id if is_initial_viewer else ""),
                }
                if game == "th06":
                    query["runtimeBuild"] = runtime_build
                if not is_initial_viewer:
                    query["stage"] = "1"
                if retry_continue and game == "th06":
                    query.update({"frames": 1200, "retry": 1})
                if is_initial_viewer:
                    query.update({"spectator": 1, "spectatorId": spectator_id, "replayFixture": 1})
                page.goto(f"{host}?{urllib.parse.urlencode(query)}", wait_until="load", timeout=30_000)

            player_pages = pages[:player_count]
            initial_viewer = pages[player_count]
            viewers = [initial_viewer]

            checkpoint = 900 if retry_continue and game == "th06" else 300
            endpoint_artifacts = [snapshot(page, checkpoint) for page in pages]
            assert_served_runtime(endpoint_artifacts, game, runtime_build,
                                  runtime_wasm_sha256, "initial")
            print(f"{game.upper()} served runtime artifact identity: "
                  f"{[(v['servedRuntimeBuild'], v['servedWasmUrl'], v['servedWasmSha256'], v['runtimePath']) for v in endpoint_artifacts]}",
                  flush=True)
            deadline = time.time() + 180
            values = None
            next_status = time.time()
            while time.time() < deadline:
                values = [snapshot(page, checkpoint) for page in pages]
                if time.time() >= next_status:
                    print(f"{game.upper()} readiness telemetry: elapsed={180 - max(0, deadline - time.time()):.1f}s "
                          f"renderer={values[0]['renderer']!r} "
                          f"pages={[(v['launched'],v['active'],v['spectator'],v['frame'],v['confirmed'],v['route'],v['failed'],v['error']) for v in values]}", flush=True)
                    next_status = time.time() + 5
                if any(failures) or any(value["hostFailure"] or value["failed"] for value in values):
                    break
                if all(value["frame"] > checkpoint and value["hash"] for value in values):
                    break
                time.sleep(0.1)
            if values is None:
                raise AssertionError("no runtime snapshots")
            if any(failures):
                raise AssertionError(f"page failures: {failures} values={values}")
            if any(value["hostFailure"] or value["failed"] for value in values):
                raise AssertionError(f"runtime failure: {values}")
            if not all(value["frame"] > checkpoint and value["hash"] for value in values):
                raise AssertionError(f"checkpoint timeout: {values}")
            print(f"{game.upper()} native checkpoint ready: renderer={values[0]['renderer']!r} "
                  f"frames={[value['frame'] for value in values]} confirmed={[value['confirmed'] for value in values]} "
                  f"hashes={[value['hash'] for value in values]}", flush=True)
            if len({value["hash"] for value in values}) != 1:
                raise AssertionError(f"spectator state divergence: {values}")
            if values[player_count]["spectator"] is not True or values[player_count]["route"] != "spectator":
                raise AssertionError(f"spectator route missing: {values[player_count]}")
            if values[player_count]["peerCount"] != 0 or values[player_count]["canSend"]:
                raise AssertionError(f"spectator gained gameplay transport: {values[player_count]}")
            if any(value["spectator"] for value in values[:player_count]):
                raise AssertionError(f"player entered spectator path: {values[:player_count]}")
            audit_frame = 320
            audit_deadline = time.time() + 20.0
            audit_values = None
            while time.time() < audit_deadline:
                audit_values = [snapshot(page, audit_frame) for page in pages]
                if (all(value["hash"] for value in audit_values) and
                        all(value["confirmed"] >= audit_frame for value in audit_values[:player_count]) and
                        audit_values[player_count]["frame"] >= audit_frame):
                    break
                time.sleep(0.1)
            if not audit_values or not (
                    all(value["hash"] for value in audit_values) and
                    all(value["confirmed"] >= audit_frame for value in audit_values[:player_count]) and
                    audit_values[player_count]["frame"] >= audit_frame):
                raise AssertionError(f"confirmed 20-frame audit sample missing at {audit_frame}: {audit_values}")
            if any(not value["stageTelemetryReady"] or value["highestStage"] < 1
                   for value in audit_values[:player_count]):
                raise AssertionError(f"native Stage 1 telemetry missing at {audit_frame}: {audit_values[:player_count]}")
            audit_hashes = [value["hash"] for value in audit_values]
            if len(set(audit_hashes)) != 1:
                raise AssertionError(f"20-frame rollback audit mismatch at {audit_frame}: {audit_values}")
            print(f"{game.upper()} Stage 1 rollback audit: PASS frame={audit_frame} "
                  f"hash={audit_hashes[0]} confirmed={[value['confirmed'] for value in audit_values[:player_count]]} "
                  f"viewerFrame={audit_values[player_count]['frame']}", flush=True)
            initial_playback = snapshot(initial_viewer, checkpoint).get("playback") or {}
            if initial_playback.get("run"):
                spectator_command(initial_viewer, game, initial_playback["run"], "live")
                live_state = wait_playback(
                    initial_viewer, lambda state: state.get("following") is True and not state.get("seeking"))
                source_frames_at_follow = [snapshot(page, checkpoint)["frame"] for page in player_pages]
                source_frontier_at_follow = max(source_frames_at_follow, default=0)
                if (not live_state.get("following") or live_state.get("seeking") or
                        live_state.get("position", 0) >= 3000 or source_frontier_at_follow >= 3000):
                    raise AssertionError(
                        f"{game.upper()} Stage 1 viewer was not following before the native fixture: "
                        f"playback={live_state} sourceFrames={source_frames_at_follow}")
                print(f"{game.upper()} initial spectator following before native Stage 1 fixture: PASS "
                      f"position={live_state['position']} sources={source_frames_at_follow} "
                      f"fixtureThreshold=3000", flush=True)
            else:
                raise AssertionError(f"{game.upper()} initial spectator playback has no run identity: {initial_playback}")
            if retry_continue and game == "th06":
                if any(
                    not value["teamWipe"] or not value["teamWipeRetry"] or
                    value["teamWipeRetryFrame"] - value["teamWipeStartFrame"] != 180 or
                    not value["retryContinue"] or not value["retryResourcesReset"] or
                    value["retryContinueFrame"] <= value["teamWipeRetryFrame"]
                    for value in values
                ):
                    raise AssertionError(f"spectator Retry lifecycle not completed: {values}")
            expected_player_route = "relay" if force_relay else "rtc"
            if any(value["route"] != expected_player_route for value in values[:player_count]):
                raise AssertionError(f"unexpected player route: {values[:player_count]}")
            if spectator_delay_ms > 0:
                player_front = max(value["frame"] for value in values[:player_count])
                if player_front - values[player_count]["frame"] > 102:
                    raise AssertionError(f"spectator failed to catch up after delayed start: {values}")
            stage_deadline = time.time() + 600
            next_status = time.time()
            while time.time() < stage_deadline:
                values = [snapshot(page, 300) for page in pages]
                if time.time() >= next_status:
                    publish = values[0].get("publish") or {}
                    print(f"{game.upper()} Stage 2 telemetry: elapsed={600 - max(0, stage_deadline - time.time()):.1f}s "
                          f"renderer={values[0]['renderer']!r} "
                          f"pages={[(v['frame'],v['confirmed'],v['highestStage'],v['stageTransition'],v['rollback'],v['failed'],v['error']) for v in values]} "
                          f"publish={publish} playback={values[player_count].get('playback')}", flush=True)
                    next_status = time.time() + 5
                if any(value["hostFailure"] or value["failed"] for value in values):
                    raise AssertionError(f"runtime failure before stage 2: {values}")
                if debug_fast_restore:
                    publish = values[0].get("publish") or {}
                    if all(value["highestStage"] >= 2 and value["stageTransition"] for value in values[:player_count]) and \
                            int(publish.get("cursor", 0)) >= 6681:
                        print(f"{game.upper()} debug fast restore admission at player frames="
                              f"{[value['frame'] for value in values[:player_count]]} publish={publish}")
                        break
                elif all(value["highestStage"] >= 2 and value["stageTransition"] for value in values[:player_count]):
                    break
                time.sleep(0.2)
            # The private test fixture drives source players through the
            # normal native boss-death/result lifecycle. The original viewer
            # receives only published inputs/checkpoints and must follow the
            # new segment automatically; it is not given the private action.
            stage_ready = values[:player_count]
            if not all(value["highestStage"] >= 2 and value["stageTransition"] for value in stage_ready):
                raise AssertionError(f"native stage 2 was not reached: {values}")
            late_id = "spectator_0004"
            admission = admit_late_spectator(setup, f"ws://127.0.0.1:{relay_port}", room,
                                             late_id, player_count)
            if admission.get("serial") != 1:
                raise AssertionError(f"late spectator admission failed: {admission}")
            late_context = browser.new_context()
            late_viewer = late_context.new_page()
            viewers.append(late_viewer)
            late_viewer.add_init_script("""(() => {
              globalThis.__spectatorPlaybackReport = null;
              addEventListener('message', event => {const m=event.data||{};
                if(m.protocol==='eagler-touhou/1'&&m.game===EXPECTED_GAME&&m.epoch===1&&
                   m.event==='runtime-info') {
                  if (m.renderer) globalThis.__runtimeRenderer=String(m.renderer);
                  if (m.spectatorPlayback?.version===1)
                    globalThis.__spectatorPlaybackReport=m.spectatorPlayback;
                }});
            })();""".replace("EXPECTED_GAME", repr(game)))
            late_viewer.on("console", lambda msg: browser_console(msg, player_count + 1))
            late_failure_index = len(failures)
            failures.append("")
            late_viewer.on("pageerror", lambda error, i=late_failure_index:
                    failures.__setitem__(i, f"late spectator: {error}"))
            query = {
                "player": 0, "players": player_count,
                "relay": relay_query + "&member=member_view_0004&spectator=" + late_id,
                "spectator": 1, "spectatorId": late_id,
            }
            if game == "th06":
                query["runtimeBuild"] = runtime_build
            late_viewer.goto(f"{host}?{urllib.parse.urlencode(query)}", wait_until="load", timeout=30_000)
            late_artifact = snapshot(late_viewer, 300)
            assert_served_runtime([late_artifact], game, runtime_build,
                                  runtime_wasm_sha256, "late-viewer")
            print(f"{game.upper()} late-viewer served runtime identity: build={late_artifact['servedRuntimeBuild']} "
                  f"url={late_artifact['servedWasmUrl']} sha256={late_artifact['servedWasmSha256']} "
                  f"runtime={late_artifact['runtimePath']}", flush=True)
            late_deadline = time.time() + 180
            stage_node = None
            target_hash_frame = None
            next_status = time.time()
            while time.time() < late_deadline:
                late = snapshot(late_viewer, 300)
                if time.time() >= next_status:
                    print(f"{game.upper()} late-viewer telemetry: elapsed={180 - max(0,late_deadline-time.time()):.1f}s "
                          f"renderer={late['renderer']!r} frame={late['frame']} confirmed={late['confirmed']} "
                          f"active={late['active']} route={late['route']} failed={late['failed']} error={late['error']} "
                          f"playback={late.get('playback')}", flush=True)
                    next_status = time.time() + 5
                report = late.get("playback") or {}
                stage_nodes = [node for node in report.get("nodes", [])
                               if node.get("kind") == "stage" and node.get("stage") >= 2]
                if failures[late_failure_index]:
                    raise AssertionError(f"{failures[late_failure_index]} values={late}")
                if late["failed"] or late["hostFailure"]:
                    raise AssertionError(f"late viewer runtime failure: {late}")
                if stage_nodes:
                    stage_node = max(stage_nodes, key=lambda node: node["frame"])
                    # Compare the first common 20-frame canonical sample at
                    # least 600 ticks after the exact initializer boundary.
                    # Stage registration may place CPs off the audit cadence.
                    minimum_hash_frame = stage_node["frame"] + 600
                    target_hash_frame = ((minimum_hash_frame + 19) // 20) * 20
                    if late["frame"] > target_hash_frame and late["spectator"] and not late["failed"]:
                        break
                if debug_fast_restore and late["failed"]:
                    break
                time.sleep(0.2)
            if stage_node is None or target_hash_frame is None:
                source_snapshots = [snapshot(page, 300) for page in player_pages]
                failure_snapshot = {
                    "lateViewer": late,
                    "sources": source_snapshots,
                    "p0Publish": source_snapshots[0].get("publish"),
                    "p0Transport": source_snapshots[0].get("spectatorTransport"),
                }
                print(f"{game.upper()} late Stage 2 checkpoint failure snapshot: "
                      f"{json.dumps(failure_snapshot, sort_keys=True)}", flush=True)
                raise AssertionError(
                    f"late spectator did not receive a stage 2 checkpoint: {failure_snapshot}")
            if late["frame"] <= target_hash_frame:
                raise AssertionError(f"late spectator did not advance from stage 2 checkpoint: {late}; node={stage_node}")
            checkpoint_states = [snapshot(p, stage_node["frame"]) for p in player_pages]
            checkpoint_states.append(snapshot(late_viewer, stage_node["frame"]))
            checkpoint_hashes = [value["checkpointHash"] for value in checkpoint_states]
            if (not all(value["checkpointFrame"] == stage_node["frame"] for value in checkpoint_states) or
                    not all(value["confirmed"] >= stage_node["frame"]
                            for value in checkpoint_states[:player_count]) or
                    not all(checkpoint_hashes)):
                raise AssertionError(f"Stage 2 checkpoint-boundary canonical mismatch at {stage_node['frame']}: "
                                     f"{checkpoint_states}; node={stage_node}")
            if game == "th06":
                source_checkpoints = checkpoint_states[:player_count]
                capture = source_checkpoints[0]
                # TH06 captures the checkpoint before RunCalcChain applies
                # this frame's inputs. decision.predictedMask describes those
                # current inputs, not the checkpoint prefix. Trust only that
                # every source confirmed through frame-1, while keeping the
                # current input masks in the diagnostic output below.
                if (not all(value["checkpointStage"] == stage_node["stage"]
                            for value in source_checkpoints) or
                        not all(value["checkpointConfirmed"] >= stage_node["frame"] - 1
                                for value in source_checkpoints) or
                        capture["checkpointCaptureFrame"] != stage_node["frame"] or
                        capture["checkpointCaptureHash"] != checkpoint_hashes[0] or
                        capture["checkpointCaptureConfirmed"] < stage_node["frame"] - 1):
                    raise AssertionError(
                        f"TH06 native capture-time checkpoint evidence mismatch at {stage_node['frame']}: "
                        f"sources={source_checkpoints}; node={stage_node}")
            print(f"{game.upper()} Stage 2 Replay boundary: PASS frame={stage_node['frame']} "
                  f"hash={checkpoint_hashes[0]} participants={player_count}P+late-viewer")
            if game == "th06":
                print(f"TH06 native checkpoint capture: PASS frame={capture['checkpointCaptureFrame']} "
                      f"hash={capture['checkpointCaptureHash']} confirmed={capture['checkpointCaptureConfirmed']} "
                      f"predictedMask={capture['checkpointCapturePredictedMask']} "
                      f"sourceConfirmed={[value['checkpointConfirmed'] for value in source_checkpoints]} "
                      f"sourcePredictedMasks={[value['checkpointPredictedMask'] for value in source_checkpoints]}")
            compared = [snapshot(p, target_hash_frame) for p in player_pages]
            compared.append(snapshot(late_viewer, target_hash_frame))
            hashes = [value["hash"] for value in compared]
            if (not all(hashes) or len(set(hashes)) != 1 or
                    not all(value["frame"] >= target_hash_frame and
                            value["confirmed"] >= target_hash_frame
                            for value in compared[:player_count])):
                raise AssertionError(f"Stage 2 restored playback diverged at {target_hash_frame}: {compared}")
            print(
                f"{game.upper()} late spectator stage 2: PASS cpFrame={stage_node['frame']} "
                f"stage={stage_node['stage']} hashFrame={target_hash_frame} "
                f"cpDelta={target_hash_frame - stage_node['frame']} "
                f"hash={hashes[0]} frames={[value['frame'] for value in compared]} admission={admission}"
            )
            spectator_command(late_viewer, game, report["run"], "live")
            late_live = wait_playback(
                late_viewer,
                lambda state: state.get("following") is True and not state.get("seeking") and
                    state.get("position", 0) >= state.get("target", 0),
                timeout=600.0)
            late_live_frame = (late_live["position"] // 20) * 20
            late_live_values = [snapshot(p, late_live_frame) for p in player_pages]
            late_live_values.append(snapshot(late_viewer, late_live_frame))
            late_live_hashes = [value["hash"] for value in late_live_values]
            if late_live_frame < target_hash_frame or not all(late_live_hashes) or \
                    len(set(late_live_hashes)) != 1:
                raise AssertionError(f"late viewer did not rejoin canonical live edge: "
                                     f"frame={late_live_frame} hashes={late_live_values} state={late_live}")
            print(f"{game.upper()} Stage 2 late viewer live: PASS frame={late_live_frame} "
                  f"hash={late_live_hashes[0]} state={late_live}")
            sustained_live = wait_live_window(
                game, player_pages, late_viewer, late_live["position"], minimum_source_advance=120,
                timeout=300.0)
            print(f"{game.upper()} Stage 2 late viewer sustained live: PASS {sustained_live}", flush=True)
            # The initial viewer started Following in Stage 1. Do not send it
            # another control here: the common playback controller must install
            # the Stage 2 checkpoint as its received input history crosses the
            # segment boundary. Verify its Replay boundary and compare future
            # gameplay against every source and the independent late viewer.
            continuous = initial_viewer
            automatic_deadline = time.time() + 600.0
            next_status = time.time()
            automatic_state = None
            automatic_checkpoint = None
            automatic_compared = None
            while time.time() < automatic_deadline:
                current_sources = [snapshot(page, target_hash_frame) for page in player_pages]
                current_continuous = snapshot(continuous, target_hash_frame)
                current_late = snapshot(late_viewer, target_hash_frame)
                current_checkpoint = snapshot(continuous, stage_node["frame"])
                state = current_continuous.get("playback") or {}
                matching_nodes = [node for node in state.get("nodes", [])
                                  if node.get("kind") == "stage" and
                                  node.get("frame") == stage_node["frame"] and
                                  node.get("stage") == stage_node["stage"]]
                if any(value["failed"] or value["hostFailure"]
                       for value in current_sources + [current_continuous, current_late]):
                    raise AssertionError(
                        f"{game.upper()} runtime failed during automatic Stage 2 following: "
                        f"sources={current_sources} continuous={current_continuous} late={current_late}")
                if time.time() >= next_status:
                    print(f"{game.upper()} automatic Stage 2 following telemetry: "
                          f"elapsed={600 - max(0, automatic_deadline - time.time()):.1f}s "
                          f"sources={[(v['frame'],v['confirmed']) for v in current_sources]} "
                          f"viewer=(frame={current_continuous['frame']},position={state.get('position')},"
                          f"edge={state.get('edge')},received={state.get('received')},"
                          f"following={state.get('following')},seeking={state.get('seeking')},"
                          f"nodes={state.get('nodes')},restoredCP={current_checkpoint['checkpointFrame']}/"
                          f"{current_checkpoint['checkpointHash']})", flush=True)
                    next_status = time.time() + 5
                source_ready = all(value["frame"] >= target_hash_frame and
                                   value["confirmed"] >= target_hash_frame and value["hash"]
                                   for value in current_sources)
                continuous_checkpoint_ready = (
                    current_checkpoint["checkpointFrame"] == stage_node["frame"] and
                    bool(current_checkpoint["checkpointHash"]) and
                    (game != "th06" or
                     current_checkpoint["checkpointStage"] == stage_node["stage"]))
                following_ready = (
                    bool(matching_nodes) and state.get("following") is True and
                    not state.get("seeking") and
                    state.get("position", 0) >= target_hash_frame and
                    current_continuous["frame"] >= target_hash_frame and
                    bool(current_continuous["hash"]))
                late_ready = (current_late["frame"] >= target_hash_frame and
                              bool(current_late["hash"]))
                if source_ready and continuous_checkpoint_ready and following_ready and late_ready:
                    automatic_compared = current_sources + [current_continuous, current_late]
                    automatic_hashes = [value["hash"] for value in automatic_compared]
                    if not all(automatic_hashes) or len(set(automatic_hashes)) != 1:
                        raise AssertionError(
                            f"{game.upper()} automatic Stage 2 follow diverged at {target_hash_frame}: "
                            f"{automatic_compared}; playback={state}; checkpoint={current_checkpoint}")
                    automatic_state = state
                    automatic_checkpoint = current_checkpoint
                    break
                time.sleep(0.2)
            if automatic_state is None or automatic_checkpoint is None or automatic_compared is None:
                raise AssertionError(
                    f"{game.upper()} original viewer did not automatically follow the native Stage 2 "
                    f"segment without a seek/live command: sourceFrame={stage_node['frame']} "
                    f"target={target_hash_frame}; playback={state}; checkpoint={current_checkpoint}; "
                    f"sources={current_sources}; late={current_late}")
            print(f"{game.upper()} original viewer automatic Stage 2 following: PASS "
                  f"cpFrame={stage_node['frame']} cpHash={automatic_checkpoint['checkpointHash']} "
                  f"hashFrame={target_hash_frame} hash={automatic_compared[0]['hash']} "
                  f"participants={player_count}P+initial-viewer+late-viewer "
                  f"state={automatic_state}", flush=True)

            continuous_state = automatic_state
            spectator_command(continuous, game, continuous_state["run"], "pause")
            paused = wait_playback(continuous, lambda state: state.get("paused") is True)
            time.sleep(0.4)
            paused_later = snapshot(continuous, 300)["playback"]
            if paused_later is None or paused_later["position"] != paused["position"] or \
                    paused_later["received"] <= paused["received"]:
                raise AssertionError(f"Stage 2 pause did not stop playback while receiving: {paused}/{paused_later}")
            spectator_command(continuous, game, paused["run"], "seek", target_hash_frame)
            stage2_seek = wait_playback(
                continuous,
                lambda state: state.get("target") == target_hash_frame and
                    not state.get("seeking") and state.get("position") == target_hash_frame,
                timeout=600.0)
            if not stage2_seek.get("paused"):
                raise AssertionError(f"Stage 2 seek did not preserve pause: {stage2_seek}")
            stage2_values = [snapshot(p, target_hash_frame) for p in player_pages]
            stage2_values += [snapshot(viewer, target_hash_frame) for viewer in viewers]
            stage2_hashes = [value["hash"] for value in stage2_values]
            if not all(stage2_hashes) or len(set(stage2_hashes)) != 1:
                raise AssertionError(f"Stage 2 seek diverged at {target_hash_frame}: {stage2_values}")
            # Source players apply the private boss-death fixture from frame
            # 3000, while viewers correctly receive only synchronized input.
            # TH07 also keeps this target before its frame-1800 size-only probe.
            # Cross back to a shared Stage 1 hash before either fixture boundary.
            cross_stage_target = 2700 if game == "th06" else 1500
            if cross_stage_target < stage2_seek["min"]:
                raise AssertionError(
                    f"continuous viewer no longer retains pre-Stage-2 history: "
                    f"target={cross_stage_target} min={stage2_seek['min']} state={stage2_seek}")
            spectator_command(continuous, game, stage2_seek["run"], "seek", cross_stage_target)
            sought = wait_playback(
                continuous,
                lambda state: state.get("target") == cross_stage_target and
                    not state.get("seeking") and state.get("position") == cross_stage_target,
                timeout=600.0)
            if not sought.get("paused"):
                raise AssertionError(f"backward seek did not preserve pause: {sought}")
            cross_values = [snapshot(p, cross_stage_target) for p in player_pages]
            cross_values.append(snapshot(continuous, cross_stage_target))
            cross_hashes = [value["hash"] for value in cross_values]
            if not all(cross_hashes) or len(set(cross_hashes)) != 1:
                raise AssertionError(
                    f"cross-stage seek diverged before Stage 2 at {cross_stage_target}: {cross_values}")
            spectator_command(continuous, game, sought["run"], "live")
            live = wait_playback(
                continuous, lambda state: state.get("following") is True and not state.get("seeking") and
                    state.get("position", 0) >= target_hash_frame and
                    state.get("position", 0) >= state.get("target", 0),
                timeout=600.0)
            live_hash_frame = (live["position"] // 20) * 20
            if live_hash_frame <= cross_stage_target:
                raise AssertionError(f"return-to-live did not advance after the cross-stage seek: {live}")
            live_values = [snapshot(p, live_hash_frame) for p in player_pages]
            live_values.append(snapshot(continuous, live_hash_frame))
            live_hashes = [value["hash"] for value in live_values]
            if not all(live_hashes) or len(set(live_hashes)) != 1:
                raise AssertionError(f"return-to-live diverged at {live_hash_frame}: {live_values}; state={live}")
            print(
                f"{game.upper()} cross-stage controls: PASS pause={paused_later['position']} "
                f"received={paused['received']}->{paused_later['received']} "
                f"seekStage2={target_hash_frame} hash={stage2_hashes[0]} "
                f"seekBeforeStage2={cross_stage_target} hash={cross_hashes[0]} "
                f"live={live_hash_frame} hash={live_hashes[0]} state={live}"
            )
            print(
                f"{game.upper()} spectator browser: PASS "
                f"playerRoute={expected_player_route} "
                f"frames={[value['frame'] for value in values]} "
                f"confirmed={[value['confirmed'] for value in values[:player_count]]} "
                f"hash{checkpoint}={values[0]['hash']}"
            )
    finally:
        for browser in reversed(browsers):
            try:
                browser.close()
            except Exception:
                pass
        relay.terminate()
        http.terminate()
        for process in (relay, http):
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()


def main() -> int:
    force_relay = "--force-relay" in sys.argv[1:] or "--rtc" not in sys.argv[1:]
    browser_channel = None
    spectator_delay_ms = 0
    retry_continue = False
    player_count = 2
    games = []
    for value in sys.argv[1:]:
        if value.startswith("--browser-channel="):
            browser_channel = value.split("=", 1)[1]
            continue
        if value in {"--rtc", "--force-relay", "--debug-fast-stage2-restore"}:
            continue
        if value == "--retry":
            retry_continue = True
            continue
        if value.startswith("--players="):
            try:
                player_count = int(value.split("=", 1)[1])
            except ValueError as exc:
                raise SystemExit("--players must be 2 or 3") from exc
            if player_count not in {2, 3}:
                raise SystemExit("--players must be 2 or 3")
            continue
        if value.startswith("--spectator-delay-ms="):
            spectator_delay_ms = max(0, int(value.split("=", 1)[1]))
            continue
        games.append(value)
    games = games or ["th06", "th07"]
    for game in games:
        if game not in {"th06", "th07"}:
            raise SystemExit(f"unknown game: {game}")
        run_game(game, force_relay, spectator_delay_ms, retry_continue,
                 browser_channel, player_count)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
