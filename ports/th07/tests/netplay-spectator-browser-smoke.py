from __future__ import annotations

import os
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


def admit_lobby(setup, relay_base: str, room: str, spectator_id: str) -> dict:
    return setup.evaluate(
        """async ({ relayBase, room, spectatorId }) => {
          function connect(id) {
            return new Promise((resolve, reject) => {
              const ws = new WebSocket(`${relayBase}/?room=${room}&lobby=${id}`);
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
              if (value.type === 'state' && active.length === 2 &&
                  Number(value.room?.spectatorCount) === 1 &&
                  active.every(seat => seat && (!requireReady || seat.ready === true))) return value;
            }
          }
          const p1 = await connect('player_one_0001'); await p1.next();
          const p2 = await connect('player_two_0002'); await p2.next();
          const spectator = await connect(spectatorId); await spectator.next();
          p1.ws.send(JSON.stringify({ type: 'take-seat', seat: 0, loadout: 0 }));
          p2.ws.send(JSON.stringify({ type: 'take-seat', seat: 1, loadout: 1 }));
          spectator.ws.send(JSON.stringify({ type: 'spectate' }));
          // Seat/loadout changes invalidate all ready votes. Wait for the
          // final roster before explicitly acknowledging its settings version.
          await nextReadyRoom(spectator, false);
          p1.ws.send(JSON.stringify({ type: 'set-ready', ready: true }));
          p2.ws.send(JSON.stringify({ type: 'set-ready', ready: true }));
          await nextReadyRoom(spectator);
          const started = nextType(spectator, 'start');
          p1.ws.send(JSON.stringify({ type: 'start' }));
          const snapshot = await started;
          globalThis.__spectatorLobbySockets = [p1.ws, p2.ws, spectator.ws];
          return { serial: snapshot.serial, spectatorCount: snapshot.room?.spectatorCount };
        }""",
        {"relayBase": relay_base, "room": room, "spectatorId": spectator_id},
    )


def snapshot(page, frame: int) -> dict:
    return page.evaluate(
        """frame => {
          const runtime = document.getElementById('runtime')?.contentWindow;
          const transport = runtime?.__th06PeerTransport || runtime?.__th07PeerTransport;
          return {
            launched: !!globalThis.__spectatorSmokeLaunched,
            hostFailure: String(globalThis.__spectatorSmokeFailure || ''),
            active: !!runtime?.__eaglerNetplayLanActive,
            spectator: !!runtime?.__eaglerNetplaySpectator,
            frame: Number(runtime?.__eaglerNetplayLanFrame || 0),
            confirmed: Number(runtime?.__eaglerNetplayLanConfirmed ?? -1),
            rollback: Number(runtime?.__eaglerNetplayLanRollback || 0),
            hash: String(runtime?.__eaglerNetplayLanHashes?.[String(frame)] || ''),
            canonical: runtime?.__eaglerNetplayLanCanonical?.[String(frame)] || null,
            canonicalMeta: runtime?.__eaglerNetplayLanCanonicalMeta?.[String(frame)] || null,
            route: String(transport?.route || ''),
            spectatorCount: Number(transport?.spectatorCount ?? -1),
            received: Number(transport?.received?.length || 0) - Number(transport?.receivedHead || 0),
            peerCount: Number(transport?.peers?.size || 0),
            canSend: typeof transport?.send === 'function' || typeof transport?.sendTo === 'function',
            failed: !!transport?.failed || !!runtime?.__eaglerNetplayFailed,
            error: String(transport?.error || runtime?.__eaglerNetplayError || ''),
            publish: runtime?.__eaglerNetplaySpectatorPublish || null,
            publishFailed: Number(runtime?.__eaglerNetplaySpectatorPublishFailed ?? -1),
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


def run_game(game: str, force_relay: bool, spectator_delay_ms: int = 0,
             retry_continue: bool = False, browser_channel: str | None = None) -> None:
    root = ROOT if game == "th07" else require_th06_root()
    http_port = free_port()
    relay_port = free_port()
    room = f"{game}-spectator-{int(time.time() * 1000)}"
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
        host = f"http://127.0.0.1:{http_port}/tests/netplay-spectator-browser-host.html"
        wait_http(host)
        wait_relay(relay)
        with sync_playwright() as pw:
            browser = pw.chromium.launch(headless=True, channel=browser_channel, args=[
                "--autoplay-policy=no-user-gesture-required",
                "--disable-background-timer-throttling",
                "--disable-backgrounding-occluded-windows",
                "--disable-renderer-backgrounding",
            ])
            browsers.append(browser)
            setup = browser.new_context().new_page()
            setup.goto(f"http://127.0.0.1:{http_port}/", wait_until="load")
            started = admit_lobby(setup, f"ws://127.0.0.1:{relay_port}", room, spectator_id)
            if started != {"serial": 1, "spectatorCount": 1}:
                raise AssertionError(f"bad lobby start: {started}")

            pages = []
            failures = [""] * 3
            for index in range(3):
                context = browser.new_context()
                if force_relay:
                    context.add_init_script("delete globalThis.RTCPeerConnection")
                page = context.new_page()
                page.on("pageerror", lambda error, i=index: failures.__setitem__(i, str(error)))
                pages.append(page)

            relay_query = f"ws://127.0.0.1:{relay_port}/?room={room}&run=1"
            for index, page in enumerate(pages):
                if index == 2 and spectator_delay_ms > 0:
                    time.sleep(spectator_delay_ms / 1000.0)
                query = {
                    "player": min(index, 1),
                    "players": 2,
                    "relay": relay_query,
                }
                if retry_continue and game == "th06":
                    query.update({"frames": 1200, "retry": 1})
                if index == 2:
                    query.update({"spectator": 1, "spectatorId": spectator_id})
                page.goto(f"{host}?{urllib.parse.urlencode(query)}", wait_until="load", timeout=30_000)

            checkpoint = 900 if retry_continue and game == "th06" else 300
            deadline = time.time() + (60 if retry_continue and game == "th06" else 30)
            values = None
            while time.time() < deadline:
                values = [snapshot(page, checkpoint) for page in pages]
                if any(failures) or any(value["hostFailure"] or value["failed"] for value in values):
                    break
                if all(value["frame"] > checkpoint and value["hash"] for value in values):
                    break
                time.sleep(0.1)
            if values is None:
                raise AssertionError("no runtime snapshots")
            if any(failures):
                raise AssertionError(f"page failures: {failures}")
            if any(value["hostFailure"] or value["failed"] for value in values):
                raise AssertionError(f"runtime failure: {values}")
            if not all(value["frame"] > checkpoint and value["hash"] for value in values):
                raise AssertionError(f"checkpoint timeout: {values}")
            if len({value["hash"] for value in values}) != 1:
                raise AssertionError(f"spectator state divergence: {values}")
            if values[2]["spectator"] is not True or values[2]["route"] != "spectator":
                raise AssertionError(f"spectator route missing: {values[2]}")
            if values[2]["peerCount"] != 0 or values[2]["canSend"]:
                raise AssertionError(f"spectator gained gameplay transport: {values[2]}")
            if any(value["spectator"] for value in values[:2]):
                raise AssertionError(f"player entered spectator path: {values[:2]}")
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
            if any(value["route"] != expected_player_route for value in values[:2]):
                raise AssertionError(f"unexpected player route: {values[:2]}")
            if spectator_delay_ms > 0:
                player_front = max(value["frame"] for value in values[:2])
                if player_front - values[2]["frame"] > 12:
                    raise AssertionError(f"spectator failed to catch up after delayed start: {values}")
            print(
                f"{game.upper()} spectator browser: PASS "
                f"playerRoute={expected_player_route} "
                f"frames={[value['frame'] for value in values]} "
                f"confirmed={[value['confirmed'] for value in values[:2]]} "
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
    force_relay = "--rtc" not in sys.argv[1:]
    browser_channel = None
    spectator_delay_ms = 0
    retry_continue = False
    games = []
    for value in sys.argv[1:]:
        if value.startswith("--browser-channel="):
            browser_channel = value.split("=", 1)[1]
            continue
        if value == "--rtc":
            continue
        if value == "--retry":
            retry_continue = True
            continue
        if value.startswith("--spectator-delay-ms="):
            spectator_delay_ms = max(0, int(value.split("=", 1)[1]))
            continue
        games.append(value)
    games = games or ["th06", "th07"]
    for game in games:
        if game not in {"th06", "th07"}:
            raise SystemExit(f"unknown game: {game}")
        run_game(game, force_relay, spectator_delay_ms, retry_continue, browser_channel)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
