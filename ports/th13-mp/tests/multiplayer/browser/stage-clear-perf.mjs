import {createRequire} from 'node:module';
import {mkdir, writeFile} from 'node:fs/promises';
import {join, resolve} from 'node:path';
import {startServe} from './serve.mjs';

const require = createRequire(import.meta.url);
const {chromium} = require(process.env.PLAYWRIGHT_MODULE || `${process.env.HOME}/work/pw/node_modules/playwright-core`);

const env = (name, fallback) => process.env[name] ?? fallback;
const stage = Number(env('TH13_STAGE', 1));
const character = Number(env('TH13_CHAR', 0));
const diff = Number(env('TH13_DIFF', 1));
const frames = Number(env('TH13_FRAMES', 300));
const after = Number(env('TH13_AFTER', 240));
const runtime = env('TH13_RUNTIME', '/artifacts/sdl3/th13-sdl.mjs');
const mode = env('TH13_MODE', 'pump');
const cpu = Number(env('TH13_CPU', 1));
const outIndex = process.argv.indexOf('--out');
const out = resolve(outIndex > 0 ? process.argv[outIndex + 1] : env('TH13_OUT', `/tmp/th13-stage-clear-${Date.now()}`));
await mkdir(out, {recursive: true});

const percentile = (sorted, p) => sorted.length ? sorted[Math.min(sorted.length - 1, Math.floor(sorted.length * p))] : 0;
const dist = values => {
  const sorted = [...values].sort((a, b) => a - b);
  const mean = sorted.length ? sorted.reduce((a, b) => a + b, 0) / sorted.length : 0;
  return {n: sorted.length, mean: +mean.toFixed(3), p50: +percentile(sorted, .5).toFixed(3),
          p95: +percentile(sorted, .95).toFixed(3), p99: +percentile(sorted, .99).toFixed(3),
          max: +(sorted.at(-1) ?? 0).toFixed(3)};
};

const server = await startServe();
let browser = null;
const logs = [];
try {
  browser = await chromium.launch({
    executablePath: env('CHROME_PATH', '/usr/local/bin/google-chrome'),
    headless: true,
    args: ['--no-sandbox', '--disable-dev-shm-usage', '--ignore-gpu-blocklist',
           '--use-angle=swiftshader', '--use-gl=angle', '--enable-unsafe-swiftshader',
           '--autoplay-policy=no-user-gesture-required',
           '--disable-background-timer-throttling', '--disable-backgrounding-occluded-windows',
           '--disable-renderer-backgrounding', ...env('TH13_CHROME_FLAGS', '').split(' ').filter(Boolean)],
  });
  const context = await browser.newContext({viewport: {width: 800, height: 700}});
  const page = await context.newPage();
  page.on('pageerror', error => logs.push({kind: 'pageerror', text: String(error)}));
  page.on('console', message => {
    if (['error', 'warning'].includes(message.type())) logs.push({kind: message.type(), text: message.text().slice(0, 300)});
  });
  if (cpu > 1) {
    const session = await context.newCDPSession(page);
    await session.send('Emulation.setCPUThrottlingRate', {rate: cpu});
  }
  const query = new URLSearchParams({stage, char: character, diff, frames, after, runtime, mode});
  await page.goto(`${server.origin}/bench/stage-clear.html?${query}`);
  const deadline = Date.now() + 600_000;
  for (;;) {
    const state = await page.evaluate(() => ({done: window.__probe.done, failure: window.__probe.failure,
      samples: window.__probe.samples.length}));
    if (state.failure) throw new Error('client failed: ' + state.failure);
    if (state.done) break;
    if (Date.now() > deadline) throw new Error('probe timeout at ' + state.samples + ' samples');
    await new Promise(done => setTimeout(done, 1000));
  }
  const samples = await page.evaluate(() => window.__probe.samples);
  await writeFile(join(out, 'samples.json'), JSON.stringify(samples));
  const inTransition = row => row.b_clear_age >= 0 || row.b_transition || row.b_loading ||
    row.b_pending >= 0 || row.b_pending_load > 0;
  const ordinary = samples.filter(row => row.run && !inTransition(row) && !row.loading_waiting).map(row => row.ms);
  const sequence = samples.filter(inTransition);
  const label = row => row.b_loading ? 'door'
    : row.b_transition && row.b_startup_age < 0 && row.startup_age >= 0 ? 'gate'
    : row.b_transition && row.b_startup_age === 30 ? 'activate'
    : row.b_transition ? `gate+${row.b_startup_age}`
    : row.b_pending >= 0 || row.b_pending_load > 0 ? 'door-pending' : `clear+${row.b_clear_age}`;
  const spikes = sequence.map((row, index) => ({index, ms: row.ms, label: label(row), stage: row.stage,
      stage_frame: row.stage_frame})).sort((a, b) => b.ms - a.ms).slice(0, 12);
  const named = {};
  for (const name of ['door', 'gate', 'activate']) {
    const row = sequence.find(row => label(row) === name);
    named[name] = row ? row.ms : null;
  }
  const report = {
    config: {stage, character, diff, frames, after, runtime, mode, cpu},
    ordinary: dist(ordinary),
    transition: {frames: sequence.length, cost: dist(sequence.map(row => row.ms)), named, spikes},
    logs: logs.slice(0, 200),
  };
  await writeFile(join(out, 'report.json'), JSON.stringify(report, null, 2));
  await page.screenshot({path: join(out, 'end.png')}).catch(() => {});
  console.log(JSON.stringify(report, null, 2));
  console.log('report:', join(out, 'report.json'));
} finally {
  if (browser) await browser.close().catch(() => {});
  await server.close().catch(() => {});
}
