// Real-browser multiplayer performance probe for boss cards.
//
// Two (or three) headless Chromium clients join a local relay, start the same
// production session inside one Spell Practice card (or a stage from frame 0)
// and run the real rAF loop with a scripted pilot. The probe reports callback
// cost, logical-frame throughput (game speed), stalls, rollbacks and
// presentation cadence per client.
//
//   TH13_STAGE=3 TH13_SLOT=3 TH13_DURATION=40 TH13_CPU=4,1 \
//     node tests/multiplayer/browser/mp-spell-perf.mjs [--out dir]
//
// Environment: TH13_STAGE (default 3), TH13_SLOT (spell slot, -1 = whole stage),
// TH13_PLAYERS (2), TH13_DIFF (3 = Lunatic), TH13_DURATION (seconds, 40),
// TH13_WARMUP (seconds, 8), TH13_CPU (per-seat CDP CPU throttle, e.g. "4,1"),
// TH13_DELAY (input delay frames, 0), TH13_GOV (1 = frame-rate-drop handling, 0 = off), TH13_PROFILE (seat to CPU-profile, -1 = off),
// TH13_CHROME_FLAGS (extra space-separated Chrome flags), PLAYWRIGHT_MODULE, CHROME_PATH.
import {createRequire} from 'node:module';
import {spawn} from 'node:child_process';
import {mkdir, readFile, writeFile} from 'node:fs/promises';
import {join, resolve} from 'node:path';
import {startServe, REPO_ROOT} from './serve.mjs';

const require = createRequire(import.meta.url);
const {chromium} = require(process.env.PLAYWRIGHT_MODULE || `${process.env.HOME}/work/pw/node_modules/playwright-core`);

const env = (name, fallback) => process.env[name] ?? fallback;
const stage = Number(env('TH13_STAGE', 3));
const slot = Number(env('TH13_SLOT', 3));
const players = Number(env('TH13_PLAYERS', 2));
const diff = Number(env('TH13_DIFF', 3));
const duration = Number(env('TH13_DURATION', 40));
const warmup = Number(env('TH13_WARMUP', 8));
const delay = Number(env('TH13_DELAY', 0));
const governor = Number(env('TH13_GOV', 1));
const cpu = env('TH13_CPU', '4,1').split(',').map(Number);
const profileSeat = Number(env('TH13_PROFILE', -1));
const chars = [0, 1, 3];
const outIndex = process.argv.indexOf('--out');
const out = resolve(outIndex > 0 ? process.argv[outIndex + 1] : env('TH13_OUT', `/tmp/th13-spell-perf-${Date.now()}`));
await mkdir(out, {recursive: true});

async function startRelay() {
  const entry = join(REPO_ROOT, 'services/relay/src/netplay-relay.mjs');
  const child = spawn(process.execPath, [entry], {
    cwd: REPO_ROOT,
    env: {...process.env, EAGLER_NETPLAY_RELAY_HOST: '127.0.0.1', EAGLER_NETPLAY_RELAY_PORT: '0',
          EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK: '1'},
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  let log = '';
  child.stderr.on('data', chunk => { log += chunk; });
  const port = await new Promise((resolvePort, reject) => {
    const timer = setTimeout(() => reject(new Error('relay timeout ' + log)), 15000);
    child.stdout.on('data', chunk => {
      const match = String(chunk).match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);
      if (match) { clearTimeout(timer); resolvePort(Number(match[1])); }
    });
  });
  return {url: `ws://127.0.0.1:${port}`, close: () => child.kill()};
}

const percentile = (sorted, p) => sorted.length ? sorted[Math.min(sorted.length - 1, Math.floor(sorted.length * p))] : 0;
const dist = values => {
  const sorted = [...values].sort((a, b) => a - b);
  const mean = sorted.length ? sorted.reduce((a, b) => a + b, 0) / sorted.length : 0;
  return {n: sorted.length, mean: +mean.toFixed(3), p50: +percentile(sorted, .5).toFixed(3),
          p95: +percentile(sorted, .95).toFixed(3), p99: +percentile(sorted, .99).toFixed(3),
          max: +(sorted.at(-1) ?? 0).toFixed(3)};
};

const server = await startServe();
const relay = await startRelay();
const browsers = [];
const pages = [];
const sessions = [];
const room = 'bench' + Math.random().toString(36).slice(2, 8);
const logs = [];
try {
  for (let seat = 0; seat < players; ++seat) {
    const browser = await chromium.launch({
      executablePath: env('CHROME_PATH', '/usr/local/bin/google-chrome'),
      headless: true,
      args: ['--no-sandbox', '--disable-dev-shm-usage', '--ignore-gpu-blocklist',
             '--use-angle=swiftshader', '--use-gl=angle', '--enable-unsafe-swiftshader',
             '--autoplay-policy=no-user-gesture-required',
             '--disable-background-timer-throttling', '--disable-backgrounding-occluded-windows',
             '--disable-renderer-backgrounding', ...env('TH13_CHROME_FLAGS', '').split(' ').filter(Boolean)],
    });
    browsers.push(browser);
    const context = await browser.newContext({viewport: {width: 800, height: 700}});
    const page = await context.newPage();
    page.on('pageerror', error => logs.push({seat, kind: 'pageerror', text: String(error)}));
    page.on('console', message => {
      if (['error', 'warning'].includes(message.type())) logs.push({seat, kind: message.type(), text: message.text().slice(0, 300)});
    });
    const session = await context.newCDPSession(page);
    const rate = cpu[seat] ?? 1;
    if (rate > 1) await session.send('Emulation.setCPUThrottlingRate', {rate});
    pages.push(page);
    sessions.push(session);
    const query = new URLSearchParams({seat, players, chars: chars.join(','), room, relay: relay.url,
      stage, slot, diff, delay, protect: 1, gov: governor, runtime: env('TH13_RUNTIME', '/artifacts/sdl3/th13-sdl.mjs')});
    await page.goto(`${server.origin}/bench/mp-bench.html?${query}`);
  }
  const deadline = Date.now() + 240_000;
  while (Date.now() < deadline) {
    const states = await Promise.all(pages.map(page => page.evaluate(() => ({
      started: window.__bench.started, failure: window.__bench.failure}))));
    const failed = states.find(state => state.failure);
    if (failed) throw new Error('client failed: ' + failed.failure);
    if (states.every(state => state.started)) break;
    await new Promise(done => setTimeout(done, 500));
  }
  console.log('session started; warming up', warmup, 's');
  await new Promise(done => setTimeout(done, warmup * 1000));

  const netState = page => page.evaluate(() => {
    const m = window.mod;
    return {
      net: JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state())),
      rollback: JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_rollback())),
      stageFrame: m._th13_debug_stage_frame(),
      readoutFps: m._th13_debug_presentation_fps ? +m._th13_debug_presentation_fps().toFixed(1) : -1,
      now: performance.now(),
    };
  });
  const mark = await Promise.all(pages.map(async page => ({
    state: await netState(page),
    samples: await page.evaluate(() => window.__bench.samples.length),
  })));
  const timeline = [];
  if (profileSeat >= 0) {
    await sessions[profileSeat].send('Profiler.enable');
    await sessions[profileSeat].send('Profiler.setSamplingInterval', {interval: 250});
    await sessions[profileSeat].send('Profiler.start');
  }
  const started = Date.now();
  while (Date.now() - started < duration * 1000) {
    await new Promise(done => setTimeout(done, 5000));
    const states = await Promise.all(pages.map(netState));
    const cadence = await Promise.all(pages.map(page => page.evaluate(() => ({
      raf: window.__bench.raf, rafGapMax: Math.round(window.__bench.rafGapMax),
      timers: window.__bench.timeouts, timerGapMax: Math.round(window.__bench.timerGapMax || 0)}))));
    timeline.push(states.map((s, index) => ({sim: s.net.sim_frame, confirmed: s.net.confirmed, stage: s.stageFrame,
      restores: s.rollback.restores, stall: s.rollback.stall_frames, stretch: s.net.stretch, fps: s.readoutFps,
      raf: cadence[index].raf, rafGap: cadence[index].rafGapMax, timerGap: cadence[index].timerGapMax,
      err: s.net.error})));
    console.log(JSON.stringify(timeline.at(-1)));
    const failed = states.find(s => s.net.error);
    if (failed) throw new Error('session error: ' + failed.net.error);
  }
  if (profileSeat >= 0) {
    const {profile} = await sessions[profileSeat].send('Profiler.stop');
    const symbols = new Map((await readFile(join(REPO_ROOT, 'ports/th13-mp/web/artifacts/sdl3/th13-sdl.mjs.symbols'), 'utf8'))
      .split('\n').filter(Boolean).map(line => { const at = line.indexOf(':'); return [line.slice(0, at), line.slice(at + 1)]; }));
    const byId = new Map(profile.nodes.map(node => [node.id, node]));
    const self = new Map();
    let total = 0;
    profile.samples.forEach((id, index) => {
      const frame = byId.get(id).callFrame;
      const match = /^wasm-function\[(\d+)\]$/.exec(frame.functionName);
      const name = match ? (symbols.get(match[1]) ?? frame.functionName) : (frame.functionName || '(anonymous)');
      const dt = profile.timeDeltas[index] / 1000;
      self.set(name, (self.get(name) ?? 0) + dt);
      total += dt;
    });
    const rows = [...self.entries()].sort((a, b) => b[1] - a[1]).slice(0, 70)
      .map(([name, ms]) => `${(ms / total * 100).toFixed(2).padStart(6)}%  ${ms.toFixed(0).padStart(7)} ms  ${name}`);
    await writeFile(join(out, `profile-seat${profileSeat}.txt`), rows.join('\n') + '\n');
    await writeFile(join(out, `profile-seat${profileSeat}.cpuprofile`), JSON.stringify(profile));
    console.log(rows.slice(0, 30).join('\n'));
  }
  const report = {config: {stage, slot, players, diff, duration, warmup, delay, cpu, governor}, clients: []};
  for (let seat = 0; seat < players; ++seat) {
    const page = pages[seat];
    const end = await netState(page);
    const samples = await page.evaluate(first => window.__bench.samples.slice(first), mark[seat].samples);
    const wall = (end.now - mark[seat].state.now) / 1000;
    const frames = end.net.sim_frame - mark[seat].state.net.sim_frame;
    const active = samples.filter(row => row.ok && row.src === 1);
    const durations = active.map(row => row.e - row.b);
    const presented = active.filter(row => row.pr > 0).map(row => row.e);
    const intervals = presented.slice(1).map((value, index) => value - presented[index]);
    report.clients.push({
      seat, cpuRate: cpu[seat] ?? 1,
      gameSpeed: +(frames / wall / 60).toFixed(4),
      logicalFps: +(frames / wall).toFixed(2),
      wallSeconds: +wall.toFixed(2),
      callbacks: dist(durations),
      over16ms: +(durations.filter(value => value > 16.7).length / Math.max(1, durations.length)).toFixed(4),
      over33ms: +(durations.filter(value => value > 33.4).length / Math.max(1, durations.length)).toFixed(4),
      simPerCallback: dist(active.map(row => row.sim)),
      zeroSimCallbacks: active.filter(row => row.sim === 0).length,
      presentIntervalMs: dist(intervals),
      readoutFps: end.readoutFps,
      readoutFpsTimeline: timeline.map(row => row[seat].fps),
      rollbackRestores: end.rollback.restores - mark[seat].state.rollback.restores,
      resimulated: end.rollback.resimulated - mark[seat].state.rollback.resimulated,
      maxDistance: end.rollback.maxDistance,
      stallFrames: end.rollback.stall_frames - mark[seat].state.rollback.stall_frames,
      errors: end.net.error,
    });
    await writeFile(join(out, `samples-${seat}.json`), JSON.stringify(samples));
  }
  report.timeline = timeline;
  report.logs = logs.slice(0, 40);
  await writeFile(join(out, 'report.json'), JSON.stringify(report, null, 2));
  for (let seat = 0; seat < players; ++seat)
    await pages[seat].screenshot({path: join(out, `seat${seat}.png`)}).catch(() => {});
  console.log(JSON.stringify(report.clients, null, 2));
  console.log('report:', join(out, 'report.json'));
} finally {
  for (const browser of browsers) await browser.close().catch(() => {});
  relay.close();
  await server.close().catch(() => {});
}
