// Real-browser render workload probe (single-player path shared by both ports).
//
// A headless Chromium client boots the production runtime, enters a Spell
// Practice card (or a stage) and pumps deterministic frames with a scripted
// pilot. Every frame records its CPU cost and the renderer's submission
// statistics (draw calls, batches, vertex bytes, binds, buffer replacements).
// The workload numbers are deterministic and independent of this machine's
// GPU, so a card's GPU submission pressure can be read and compared directly.
//
//   TH13_STAGE=3 TH13_SLOT=3 TH13_DIFF=3 node tests/multiplayer/browser/render-stats-perf.mjs [--out dir]
//
// Environment: TH13_STAGE (3), TH13_SLOT (spell slot, -1 = whole stage, 3),
// TH13_CHAR (0), TH13_DIFF (3 = Lunatic), TH13_FRAMES (1800), TH13_RUNTIME
// (module path; /sp/artifacts/sdl3/th13-sdl.mjs for ports/th13), TH13_PRESENT
// (1 = present every frame), TH13_CHROME_FLAGS, PLAYWRIGHT_MODULE, CHROME_PATH.
import {createRequire} from 'node:module';
import {mkdir, writeFile} from 'node:fs/promises';
import {join, resolve} from 'node:path';
import {startServe} from './serve.mjs';

const require = createRequire(import.meta.url);
const {chromium} = require(process.env.PLAYWRIGHT_MODULE || `${process.env.HOME}/work/pw/node_modules/playwright-core`);

const env = (name, fallback) => process.env[name] ?? fallback;
const stage = Number(env('TH13_STAGE', 3));
const slot = Number(env('TH13_SLOT', 3));
const character = Number(env('TH13_CHAR', 0));
const diff = Number(env('TH13_DIFF', 3));
const frames = Number(env('TH13_FRAMES', 1800));
const runtime = env('TH13_RUNTIME', '/artifacts/sdl3/th13-sdl.mjs');
const present = Number(env('TH13_PRESENT', 0));
const outIndex = process.argv.indexOf('--out');
const out = resolve(outIndex > 0 ? process.argv[outIndex + 1] : env('TH13_OUT', `/tmp/th13-render-stats-${Date.now()}`));
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
  const query = new URLSearchParams({stage, slot, char: character, diff, frames, runtime, present});
  await page.goto(`${server.origin}/bench/render-stats.html?${query}`);
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
  // Frames where the stage clock advanced are live gameplay; the pilot is
  // scripted and may die, after which the game idles on a prompt and keeps
  // redrawing the same scene. Only live frames describe the card's workload.
  const live = samples.filter((row, index) => index === 0 || row.stage_frame !== samples[index - 1].stage_frame);
  const column = name => live.map(row => row[name]);
  const peak = live.reduce((best, row) => row.calls > best.calls ? row : best, live[0]);
  const report = {
    config: {stage, slot, character, diff, frames, runtime, present},
    liveFrames: live.length,
    lastStageFrame: samples.at(-1)?.stage_frame ?? 0,
    ms: dist(column('ms')),
    calls: dist(column('calls')),
    batches: dist(column('batches')),
    genericBatches: dist(column('genericBatches')),
    vertexUploadBytes: dist(column('vertexUploadBytes')),
    uploadBytes: dist(column('uploadBytes')),
    bufferReplacements: dist(column('bufferReplacements')),
    bufferSubUpdates: dist(column('bufferSubUpdates')),
    textureBinds: dist(column('textureBinds')),
    framebufferBinds: dist(column('framebufferBinds')),
    layoutSetups: dist(column('layoutSetups')),
    programCompiles: column('programCompiles').reduce((a, b) => a + b, 0),
    peakFrame: peak,
    logs: logs.slice(0, 50),
  };
  await writeFile(join(out, 'report.json'), JSON.stringify(report, null, 2));
  await page.screenshot({path: join(out, 'end.png')}).catch(() => {});
  console.log(JSON.stringify(report, null, 2));
  console.log('report:', join(out, 'report.json'));
} finally {
  if (browser) await browser.close().catch(() => {});
  await server.close().catch(() => {});
}
