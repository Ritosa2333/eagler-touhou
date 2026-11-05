from __future__ import annotations
import os, socket, subprocess, sys, time
from pathlib import Path
from integration_support import require_host_relay
import numpy as np
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]
HOST_ROOT, RELAY_SCRIPT = require_host_relay()


def free_port():
    with socket.socket() as s:
        s.bind(('127.0.0.1', 0))
        return s.getsockname()[1]


def wait_http(url, timeout=10):
    import urllib.request
    end = time.time() + timeout
    while time.time() < end:
        try:
            with urllib.request.urlopen(url, timeout=.5) as r:
                if r.status < 400:
                    return
        except Exception:
            time.sleep(.1)
    raise RuntimeError('http timeout')


def wait_relay(proc, timeout=10):
    end = time.time() + timeout
    while time.time() < end:
        line = proc.stdout.readline()
        if line:
            print('RELAY', line.rstrip(), flush=True)
            if 'LAN relay listening' in line:
                return
        elif proc.poll() is not None:
            raise RuntimeError('relay exited')
    raise RuntimeError('relay timeout')


def runtime(page):
    return page.evaluate("""() => {
      const w = document.getElementById('runtime')?.contentWindow;
      return {
        frame: Number(w?.__eaglerNetplayLanFrame || 0),
        rollback: Number(w?.__eaglerNetplayLanRollback || 0),
        failed: !!w?.__eaglerNetplayFailed,
        error: String(w?.__eaglerNetplayError || '')
      };
    }""")


def install_tap(page):
    return page.evaluate("""() => {
      const w = document.getElementById('runtime')?.contentWindow;
      const s = w?.Module?.SDL3;
      const node = s?.audio_playback?.scriptProcessorNode;
      if (!node || typeof node.onaudioprocess !== 'function') return false;
      if (w.__bombAbTapInstalled) return true;
      const original = node.onaudioprocess;
      w.__bombAbTap = { sampleRate: Number(s.audioContext?.sampleRate || 0), capture: false, blocks: [] };
      node.onaudioprocess = function(e) {
        original.call(this, e);
        const tap = w.__bombAbTap;
        if (!tap?.capture) return;
        tap.blocks.push({
          frame: Number(w.__eaglerNetplayLanFrame || 0),
          rollback: Number(w.__eaglerNetplayLanRollback || 0),
          samples: new Float32Array(e.outputBuffer.getChannelData(0))
        });
      };
      w.__bombAbTapInstalled = true;
      return true;
    }""")


def start_capture(page):
    page.evaluate("""() => {
      const w = document.getElementById('runtime')?.contentWindow;
      if (w?.__bombAbTap) { w.__bombAbTap.blocks = []; w.__bombAbTap.capture = true; }
    }""")


def stop_capture(page):
    page.evaluate("""() => {
      const w = document.getElementById('runtime')?.contentWindow;
      if (w?.__bombAbTap) w.__bombAbTap.capture = false;
    }""")


def get_blocks(page):
    raw = page.evaluate("""() => {
      const w = document.getElementById('runtime')?.contentWindow;
      const t = w?.__bombAbTap;
      if (!t) return null;
      return {
        rate: t.sampleRate,
        blocks: t.blocks.map(b => ({frame:b.frame, rollback:b.rollback, samples:Array.from(b.samples)}))
      };
    }""")
    if not raw:
        raise RuntimeError('audio tap missing')
    return int(raw['rate']), [
        (int(b['frame']), int(b['rollback']), np.asarray(b['samples'], dtype=np.float32))
        for b in raw['blocks']
    ]


def key(page, down):
    page.evaluate("""down => {
      const w = document.getElementById('runtime')?.contentWindow;
      w?.dispatchEvent(new KeyboardEvent(down ? 'keydown' : 'keyup', {
        key:'x', code:'KeyX', keyCode:88, bubbles:true, cancelable:true
      }));
    }""", down)


def run_case(label, bomb):
    hp, rp = free_port(), free_port()
    env = os.environ.copy()
    env.update({
        'TH07_RELAY_HOST':'127.0.0.1',
        'TH07_RELAY_PORT':str(rp),
        'TH07_RTC_TIMEOUT_MS':'4500',
        'TH07_STUN_URLS':'',
        'TH07_RELAY_DELAY_MS':'55',
        'TH07_RELAY_JITTER_MS':'10',
    })
    http = subprocess.Popen([sys.executable, '-m', 'http.server', str(hp), '--bind', '127.0.0.1'], cwd=ROOT, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    relay = subprocess.Popen(['node', str(RELAY_SCRIPT)], cwd=HOST_ROOT, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1)
    browsers = []
    try:
        wait_http(f'http://127.0.0.1:{hp}/tests/netplay-browser-host.html')
        wait_relay(relay)
        room = f'audio-ab-{label}-{int(time.time()*1000)}'
        with sync_playwright() as pw:
            pages = []
            for i in range(2):
                b = pw.chromium.launch(headless=True, args=[
                    '--autoplay-policy=no-user-gesture-required',
                    '--disable-background-timer-throttling',
                    '--disable-backgrounding-occluded-windows',
                    '--disable-renderer-backgrounding',
                ])
                browsers.append(b)
                c = b.new_context(viewport={'width':960,'height':720})
                c.add_init_script('delete globalThis.RTCPeerConnection')
                page = c.new_page()
                pages.append(page)
                relay_url = f'ws://127.0.0.1:{rp}/?room={room}&player={i}'
                page.goto(
                    f'http://127.0.0.1:{hp}/tests/netplay-browser-host.html?player={i}&players=2&relay={relay_url}&frames=420&input=none&scripted=0',
                    wait_until='load', timeout=30000)

            armed = [False, False]
            capturing = pressed = released = False
            rollback_before = None
            press_frame = -1
            deadline = time.time() + 50
            while time.time() < deadline:
                for i, page in enumerate(pages):
                    if not armed[i]:
                        try:
                            armed[i] = bool(install_tap(page))
                        except Exception:
                            pass
                state = [runtime(p) for p in pages]
                if any(v['failed'] for v in state):
                    raise RuntimeError(state)
                if all(armed) and not capturing and min(v['frame'] for v in state) >= 130:
                    for p in pages:
                        start_capture(p)
                    capturing = True
                if bomb and capturing and not pressed and state[1]['frame'] >= 180:
                    rollback_before = state[0]['rollback']
                    press_frame = state[1]['frame']
                    key(pages[1], True)
                    pressed = True
                    print(f'{label} BOMB down P2 frame={press_frame} P1rollbackBefore={rollback_before}', flush=True)
                if bomb and pressed and not released and state[1]['frame'] >= press_frame + 3:
                    key(pages[1], False)
                    released = True
                    print(f'{label} BOMB up P2 frame={state[1]["frame"]}', flush=True)
                if min(v['frame'] for v in state) >= 265:
                    if not bomb or (released and state[0]['rollback'] > (rollback_before or 0)):
                        break
                time.sleep(.02)

            final = [runtime(p) for p in pages]
            for p in pages:
                stop_capture(p)
            print(label, 'FINAL', final, flush=True)
            if bomb:
                if not pressed or not released:
                    raise RuntimeError('Bomb key not injected')
                if final[0]['rollback'] <= (rollback_before or 0):
                    raise RuntimeError('P1 did not rollback for remote Bomb')
            data = [get_blocks(p) for p in pages]
            for i, (rate, blocks) in enumerate(data):
                print(label, f'P{i+1} rate={rate} blocks={len(blocks)} frames={[f for f,_,_ in blocks]}', flush=True)
            return data, final
    finally:
        for b in browsers:
            try:
                b.close()
            except Exception:
                pass
        relay.terminate(); http.terminate()
        try:
            relay.wait(3)
        except Exception:
            relay.kill()
        try:
            http.wait(3)
        except Exception:
            http.kill()


def metrics(blocks):
    return [(f, rb, float(np.sqrt(np.mean(x*x))), float(np.max(np.abs(x)))) for f, rb, x in blocks]


def nearest_profile(base, bomb):
    base_m = metrics(base)
    bomb_m = metrics(bomb)
    rows = []
    for bf, brb, br, bp in bomb_m:
        candidates = [r for r in base_m if abs(r[0] - bf) <= 5]
        if not candidates:
            continue
        af, arb, ar, ap = min(candidates, key=lambda r: abs(r[0] - bf))
        rows.append((bf, br-ar, bp-ap, br, bp, af))
    return rows


def cluster_positive(rows):
    active = [r for r in rows if 175 <= r[0] <= 245 and (r[1] > 0.02 or r[2] > 0.10)]
    clusters = []
    for row in active:
        if not clusters or row[0] - clusters[-1][-1][0] > 12:
            clusters.append([row])
        else:
            clusters[-1].append(row)
    return clusters


def main():
    base, _ = run_case('BASE', False)
    bomb, _ = run_case('BOMB', True)
    all_clusters = []
    for i in range(2):
        if base[i][0] != bomb[i][0]:
            raise RuntimeError('sample rate changed')
        rows = nearest_profile(base[i][1], bomb[i][1])
        print(f'=== P{i+1} bomb-minus-baseline profile ===', flush=True)
        for row in rows:
            print('DELTA', tuple(round(v, 6) if isinstance(v, float) else v for v in row), flush=True)
        clusters = cluster_positive(rows)
        all_clusters.append(clusters)
        print(f'P{i+1} activeClusters={[[r[0] for r in c] for c in clusters]}', flush=True)

    for i, clusters in enumerate(all_clusters):
        if not clusters:
            raise RuntimeError(f'P{i+1} has no Bomb-induced final PCM cluster')
        early = [c for c in clusters if c[0][0] <= 225]
        if len(early) > 1:
            raise RuntimeError(f'P{i+1} has repeated early Bomb PCM clusters')
    print('TH06 remote Bomb final WebAudio A/B: PASS', flush=True)


if __name__ == '__main__':
    main()
