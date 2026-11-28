import {createServer} from 'node:http';
import {readFile, writeFile, mkdir} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';

const args = process.argv.slice(2);
const opt = (name, fallback) => {
  const i = args.indexOf('--' + name);
  return i >= 0 && args[i + 1] !== undefined ? args[i + 1] : fallback;
};
const flag = name => args.includes('--' + name);

const root = resolve(dirname(fileURLToPath(import.meta.url)), '../../../../../..');
const portRoot = join(root, 'eagler-touhou/ports/th13');
const runtimeDir = join(portRoot, 'web/sdl-runtime');
const buildDir = process.env.TH13_SDL3_BUILD_DIR ?? join(portRoot, 'web/artifacts');
const inputsPath = resolve(root, opt('inputs', 'artifacts/tmp-scratch/e2e_inputs.json'));
const outDir = resolve(root, opt('out', 'artifacts/tmp-scratch/e2e'));
const deadlineMin = Number(opt('deadline-min', '55'));
const limitStage = Number(opt('limit-stage', '0'));
const bootSeedArg = opt('boot-seed', '');

const inputs = JSON.parse(await readFile(inputsPath, 'utf8'));
const stage1Seed = inputs.stages[0].seed;
const bootSeed = bootSeedArg === '' ? stage1Seed : Number(bootSeedArg);
if (!Number.isInteger(bootSeed) || bootSeed < 0 || bootSeed > 65535)
  throw Error('invalid --boot-seed');
const held = inputs.stages.map(s => Uint16Array.from(s.held));
const expectedScore = inputs.stages.map(s => s.start_score);
const NAME = 'ARINATEI';
const NAME_POS = {A: 0, R: 17, I: 8, N: 13, T: 19, E: 4};

const SCAN = {Z: 44, X: 45, SHIFT: 42, UP: 200, DOWN: 208, LEFT: 203, RIGHT: 205,
              ESC: 1, CTRL: 29, C: 46};

await mkdir(outDir, {recursive: true});
const log = (...xs) => console.log('[e2e', new Date().toISOString().slice(11, 19), ']', ...xs);
const timeline = [];
const mark = (event, data = {}) => {
  timeline.push({t: new Date().toISOString(), event, ...data});
  log(event, JSON.stringify(data));
};
const started = Date.now();
const deadline = started + deadlineMin * 60000;
const left = () => deadline - Date.now();
if (left() <= 0) throw Error('deadline already passed');

function nameMoves(from, to) {
  const step = c => [
    [(c + 91 - 13) % 91, SCAN.UP],
    [(c + 13) % 91, SCAN.DOWN],
    [c % 13 === 0 ? (c + 12) % 91 : (c + 91 - 1) % 91, SCAN.LEFT],
    [c % 13 === 12 ? (c + 91 - 12) % 91 : (c + 1) % 91, SCAN.RIGHT],
  ];
  const prev = new Map([[from, null]]);
  const queue = [from];
  while (queue.length) {
    const c = queue.shift();
    if (c === to) {
      const path = [];
      for (let cur = to; prev.get(cur); cur = prev.get(cur)[0]) path.unshift(prev.get(cur)[1]);
      return path;
    }
    for (const [n, scan] of step(c)) if (!prev.has(n)) { prev.set(n, [c, scan]); queue.push(n); }
  }
  throw Error(`name cursor unreachable ${from}->${to}`);
}

const mime = {'.html': 'text/html', '.mjs': 'text/javascript', '.wasm': 'application/wasm',
              '.json': 'application/json', '.ttc': 'font/ttf'};
const server = createServer(async (req, res) => {
  try {
    const url = req.url.split('?', 2)[0];
    const data = url.startsWith('/sdl-runtime/')
        ? await readFile(join(runtimeDir, url.replace('/sdl-runtime/', '')))
        : url.startsWith('/artifacts/')
          ? await readFile(join(buildDir, url.replace('/artifacts/', '')))
          : null;
    if (!data) { res.writeHead(404).end(); return; }
    res.writeHead(200, {'Content-Type': mime[url.slice(url.lastIndexOf('.'))] ?? 'application/octet-stream',
                        'Cross-Origin-Opener-Policy': 'same-origin',
                        'Cross-Origin-Embedder-Policy': 'require-corp'});
    res.end(data);
  } catch { res.writeHead(404).end(); }
});
await new Promise(done => server.listen(0, '127.0.0.1', done));
log('serving http://127.0.0.1:' + server.address().port + '/sdl-runtime/th13.html');

const require = createRequire(import.meta.url);
let chromium;
try { ({chromium} = require('playwright-core')); }
catch { ({chromium} = require(join(process.env.NODE_PATH ?? '/opt/shot/node_modules', 'playwright-core'))); }

const build = JSON.parse(await readFile(join(buildDir, 'sdl3/build.json'), 'utf8'));
const consoleLines = [];
const pageErrors = [];
let browser;
let page;
let downloadPath = null;

try {
  browser = await chromium.launch({executablePath: process.env.CHROMIUM_PATH, headless: true,
    args: ['--no-sandbox', '--disable-dev-shm-usage', '--use-angle=swiftshader',
           '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required',
           '--force-color-profile=srgb']});
  const context = await browser.newContext({viewport: {width: 800, height: 560},
                                            acceptDownloads: true});
  page = await context.newPage();
  page.on('console', m => consoleLines.push(m.text()));
  page.on('pageerror', e => pageErrors.push(String(e)));
  page.on('download', async d => {
    try {
      downloadPath = join(outDir, 'e2e_export_download.rpy');
      await d.saveAs(downloadPath);
      mark('download-captured', {suggested: d.suggestedFilename()});
    } catch (e) { mark('download-save-failed', {error: String(e)}); }
  });
  await page.addInitScript(seed => { window.__th13_boot_seed = seed; }, bootSeed);
  await page.addInitScript(() => {
    let module;
    Object.defineProperty(window, 'mod', {configurable: true,
      get() { return module; }, set(value) {
        module = value;
        const start = value._th13_loop_start;
        value._th13_loop_start = () => {
          start();
          value._th13_loop_pause(1);
          window.__e2e_loop_paused = true;
        };
      }});
  });

  const installE2e = () => page.evaluate(() => {
    window.__e2e = {
      probe() {
        const ptr = window.mod._th13_probe_state() / 4;
        return Array.from(window.mod.HEAP32.subarray(ptr, ptr + 16));
      },
      err() { return window.mod.UTF8ToString(window.mod._th13_error()); },
      key(scan, down) { window.mod._th13_key(scan, down); },
      pump(n) {
        for (let i = 0; i < n; ++i)
          if (!window.mod._th13_debug_frame()) throw new Error(window.__e2e.err());
        return window.__e2e.probe();
      },
      pumpWhile(source, cap) {
        const pred = new Function('p', 'return (' + source + ');');
        let p = null;
        for (let i = 0; i < cap; ++i) {
          p = window.__e2e.pump(1);
          if (pred(p)) return {p, pumps: i + 1, ok: true};
        }
        return {p, pumps: cap, ok: false};
      },
    };
  });

  const boot = async () => {
    await page.goto(`http://127.0.0.1:${server.address().port}/sdl-runtime/th13.html`,
                    {waitUntil: 'domcontentloaded'});
    await page.waitForFunction(
      'window.mod && window.mod._th13_phase() === 0 && window.__e2e_loop_paused === true' +
      ' && window.__th13_builtin_replays_done === true && window.__th13_replay_restore_done === true',
      null, {timeout: Math.max(30000, Math.min(300000, left()))});
    await installE2e();
  };
  await boot();
  const seedSeen = await page.evaluate(() => window.mod._th13_boot_seed());
  mark('boot-complete', {bootSeed, seen: seedSeen});
  if (seedSeen !== bootSeed) throw Error('boot seed override did not take effect');

  const probe = async () => page.evaluate(() => window.__e2e.probe());
  const pumpUntil = async (pred, cap, what) => {
    let done = 0, last;
    while (done < cap) {
      if (left() < 60000) throw Error('deadline while waiting for ' + what);
      const chunk = Math.min(4000, cap - done);
      const r = await page.evaluate(
        ({src, n}) => window.__e2e.pumpWhile(src, n), {src: pred, n: chunk});
      last = r.p;
      done += chunk;
      if (r.ok) return last;
    }
    throw Error(`pumpUntil('${what}') exhausted ${cap} pumps; probe=${JSON.stringify(last)}`);
  };
  const tap = async scan => {
    await page.evaluate(s => { window.__e2e.key(s, 1); window.__e2e.pump(1);
                               window.__e2e.key(s, 0); window.__e2e.pump(2); }, scan);
  };
  const tapOnce = async scan => {
    await page.evaluate(s => { window.__e2e.key(s, 1); window.__e2e.pump(1);
                               window.__e2e.key(s, 0); }, scan);
  };
  const shot = async name => {
    try {
      await page.screenshot({path: join(outDir, name + '.png'), timeout: 15000});
      mark('screenshot', {name});
    } catch (e) { mark('screenshot-failed', {name, error: String(e).slice(0, 120)}); }
  };

  mark('title-wait');
  await pumpUntil('p[0]===0 && p[8]===1 && p[9]===2', 1500, 'title interactive');
  await shot('01-title');
  await tap(SCAN.Z);
  await pumpUntil('p[8]===6 && p[9]===2', 400, 'difficulty screen');
  {
    let p = await probe();
    let guard = 0;
    while (p[10] !== 1 && guard++ < 4) {
      await tap(p[10] < 1 ? SCAN.DOWN : SCAN.UP);
      p = await probe();
    }
    if (p[10] !== 1) throw Error('difficulty cursor ' + p[10] + ' != 1 (Normal)');
    mark('difficulty-cursor', {cursor: p[10]});
  }
  await tap(SCAN.Z);
  mark('difficulty-confirmed');
  await pumpUntil('p[8]===7 && p[9]===2 && p[3]===1', 400, 'character screen (Normal)');
  {
    const p = await probe();
    mark('character-screen', {cursor: p[10], difficulty: p[3], character: p[4]});
  }
  await shot('02-character');
  await tapOnce(SCAN.Z);
  mark('character-confirmed');

  const game = await page.evaluate(src => {
    const held = src.map(a => Uint16Array.from(a));
    const scanFor = h => {
      const s = [];
      if (h & 0x800) s.push(46); else if (h & 0x200) s.push(29);
      if (h & 0x001) s.push(44);
      if (h & 0x002) s.push(45);
      if (h & 0x008) s.push(42);
      if (h & 0x010) s.push(200);
      if (h & 0x020) s.push(208);
      if (h & 0x040) s.push(203);
      if (h & 0x080) s.push(205);
      if (h & 0x100) s.push(1);
      return s;
    };
    const cur = new Set();
    const apply = h => {
      const want = scanFor(h);
      for (const s of [...cur]) if (!want.includes(s)) { window.mod._th13_key(s, 0); cur.delete(s); }
      for (const s of want) if (!cur.has(s)) { window.mod._th13_key(s, 1); cur.add(s); }
    };
    window.__e2e.state = {anchored: false, stream: -1, i: 0, finished: false,
                          activationPending: null, stageScores: {}, stageFrames: {},
                          pumped: 0, mismatch: null, finishedAtIndex: -1};
    window.__e2e.game = chunkMs => {
      const g = window.__e2e.state, t0 = performance.now();
      while (performance.now() - t0 < chunkMs) {
        if (g.finished) return {finished: true, g};
        if (!g.anchored) {
          if (!window.mod._th13_debug_frame()) return {failed: window.__e2e.err(), g};
          ++g.pumped;
          const p = window.__e2e.probe();
          if (p[0] === 1 && p[2] === 1) {
            g.anchored = true; g.stream = 0; g.i = 0;
            g.stageScores[0] = p[8]; g.stageFrames[0] = p[5];
          }
          continue;
        }
        const h = held[g.stream];
        if (g.i >= h.length) {
          if (g.stream === held.length - 1) {
            g.finished = true; g.finishedAtIndex = g.i;
            apply(0);
            return {finished: true, g};
          }
          g.stream += 1; g.i = 0; g.activationPending = g.stream + 1;
        }
        apply(held[g.stream][g.i]);
        ++g.i;
        if (!window.mod._th13_debug_frame()) return {failed: window.__e2e.err(), g};
        ++g.pumped;
        if (g.activationPending !== null) {
          const p = window.__e2e.probe();
          g.stageFrames[g.stream] = p[5];
          if (p[2] !== g.activationPending) {
            g.mismatch = {expectedStage: g.activationPending, gotStage: p[2],
                          stageFrame: p[5], score: p[8]};
            return {mismatch: g.mismatch, g};
          }
          g.stageScores[g.stream] = p[8];
          g.activationPending = null;
        }
      }
      return {g, p: window.__e2e.probe()};
    };
    return true;
  }, held.map(a => Array.from(a)));
  if (!game) throw Error('game harness install failed');

  let lastStage = -1;
  let lastLogAt = 0;
  for (;;) {
    if (left() < 120000) throw Error('global deadline while in gameplay');
    const r = await page.evaluate(ms => window.__e2e.game(ms), 4000);
    const g = r.g;
    if (r.failed) throw Error('runtime failure in gameplay: ' + r.failed);
    if (r.mismatch) throw Error('stage activation mismatch: ' + JSON.stringify(r.mismatch));
    if (g.stream !== lastStage) {
      const stage = g.stream + 1;
      const got = g.stageScores[g.stream];
      const want = expectedScore[g.stream];
      mark('stage-entry', {stage, score: got, expected: want, match: got === want,
                           hostPumped: g.pumped, stageFrame: g.stageFrames[g.stream],
                           elapsedMin: +((Date.now() - started) / 60000).toFixed(2)});
      lastStage = g.stream;
      if (got !== want)
        throw Error(`stage ${stage} entry score ${got} != original ${want}` +
                    '（RNG/对齐假设失败的硬证据）');
      await shot(`stage${stage}-entry`);
      if (limitStage > 0 && stage >= limitStage) {
        mark('limit-stage-reached', {stage});
        break;
      }
    } else if (Date.now() - lastLogAt > 45000) {
      lastLogAt = Date.now();
      log('progress', JSON.stringify({stage: g.stream + 1, frame: g.i, pumped: g.pumped,
                                      score: (r.p ?? [])[8], deaths: (r.p ?? [])[13],
                                      elapsedMin: +((Date.now() - started) / 60000).toFixed(2)}));
    }
    if (r.finished || g.finished) { mark('stage6-stream-exhausted', {index: g.finishedAtIndex}); break; }
  }
  if (limitStage > 0) {
    await shot('smoke-stop');
    await writeFile(join(outDir, 'e2e_report.json'),
                    JSON.stringify({mode: 'smoke', bootSeed, stage1Seed, limitStage,
                                    wasmSha256: build.sha256, timeline,
                                    consoleTail: consoleLines.slice(-80), pageErrors},
                                   null, 2) + '\n');
    mark('smoke-done');
  } else {
    let endingPumps = 0;
    for (;;) {
      if (left() < 120000) throw Error('deadline while in ending');
      const r = await page.evaluate(({cap}) => {
        const p0 = () => window.__e2e.probe();
        for (let i = 0; i < cap; ++i) {
          const p = p0();
          if (p[0] === 0 && p[8] === 14 && p[9] === 2) return {done: true, p, pumps: i};
          window.__e2e.key(44, 1);
          if (!window.mod._th13_debug_frame()) return {failed: window.__e2e.err(), p};
          window.__e2e.key(44, 0);
          for (let k = 0; k < 3; ++k) window.mod._th13_debug_frame();
        }
        return {done: false, p: p0(), pumps: cap};
      }, {cap: 3000});
      if (r.failed) throw Error('runtime failure in ending: ' + r.failed);
      endingPumps += r.pumps * 4;
      if (r.done) { mark('result-name-screen', {endingPumps}); break; }
      log('ending-progress', JSON.stringify({pumps: endingPumps, probe: r.p}));
      if (endingPumps > 300000) throw Error('ending did not finish; probe=' + JSON.stringify(r.p));
    }
    await shot('03-result-name');

    let cursor = 0;
    for (let k = 0; k < NAME.length; ++k) {
      const target = NAME_POS[NAME[k]];
      for (const mv of nameMoves(cursor, target)) await tap(mv);
      await tap(SCAN.Z);
      cursor = k === NAME.length - 1 ? 90 : target;
    }
    await tap(SCAN.Z);
    await pumpUntil('p[8]===15 && p[9]===2', 800, 'replay slot screen');
    mark('replay-slot-screen');
    await shot('04-replay-slot');
    await tap(SCAN.Z);
    await pumpUntil('p[8]===15 && p[9]===3', 800, 'replay name editor');
    await tap(SCAN.Z);
    mark('save-committed');

    let drained = await page.evaluate(() => localStorage.getItem('th13.scoredat'));
    let drainStable = 0;
    for (let chunk = 0; chunk < 20 && drainStable < 2; ++chunk) {
      await page.evaluate(n => window.__e2e.pump(n), 120);
      const now = await page.evaluate(() => localStorage.getItem('th13.scoredat'));
      if (now === drained) ++drainStable;
      else { drainStable = 0; drained = now; }
    }
    if (drainStable < 2) throw Error('scoredat mirror did not drain after save');
    mark('scoredat-mirror-drained');

    const saved = await page.evaluate(() => ({
      index: JSON.parse(localStorage.getItem('th13.replays') || '[]'),
      blob: localStorage.getItem('th13.replay.th13_01.rpy'),
      scoredat: localStorage.getItem('th13.scoredat'),
      hint: window.mod._th13_replay_import_hint(),
      phase: window.mod._th13_phase(),
      probe: window.__e2e.probe(),
    }));
    if (!saved.blob) throw Error('localStorage th13.replay.th13_01.rpy missing after save');
    if (saved.scoredat == null) throw Error('localStorage th13.scoredat missing after save');
    const blobBytes = Buffer.from(saved.blob, 'base64');
    await writeFile(join(outDir, 'e2e_export.rpy'), blobBytes);
    await writeFile(join(outDir, 'e2e_score.dat'), Buffer.from(saved.scoredat, 'base64'));
    mark('export-captured', {bytes: blobBytes.length, hint: saved.hint,
                             scoredatBytes: Buffer.from(saved.scoredat, 'base64').length,
                             entry: saved.index.find(e => e.name === 'th13_01.rpy') ?? null});
    if (saved.hint !== 0)
      mark('hint-unexpected', {hint: saved.hint, note: 'save 链 import 应置 th13_01 → 槽 0'});
    await shot('05-after-save');

    await tap(SCAN.X);
    await pumpUntil('p[8]===1 && p[9]===2', 1200, 'back to main menu');
    mark('back-to-main');
    for (let i = 0; i < 4; ++i) await tap(SCAN.DOWN);
    {
      const p = await probe();
      if (p[10] !== 4) throw Error('main menu cursor ' + p[10] + ' != 4 (Replay)');
    }
    await tap(SCAN.Z);
    await pumpUntil('p[8]===11', 1200, 'replay menu');
    await shot('06-replay-menu');
    let rp = null;
    for (let attempt = 0; attempt < 8 && !(rp && rp.active); ++attempt) {
      await page.evaluate(n => window.__e2e.pump(n), 60);
      await tap(SCAN.Z);
      rp = await page.evaluate(cap => {
        let j = null;
        for (let i = 0; i < cap; ++i) {
          if (!window.mod._th13_debug_frame()) break;
          j = JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_replay_progress()));
          if (j.active && j.slot_index === 0 && j.slot_number === 1 && j.consumed >= 30) return j;
        }
        return j ?? JSON.parse(window.mod.UTF8ToString(window.mod._th13_debug_replay_progress()));
      }, 4000);
      log('replay-playback-attempt', JSON.stringify({attempt, rp}));
    }
    mark('replay-playback', rp);
    if (!rp || !rp.active || rp.slot_index !== 0 || rp.slot_number !== 1 ||
        rp.slot_frames !== inputs.stages[0].frames)
      throw Error('replay menu playback did not open our export: ' + JSON.stringify(rp));
    await shot('07-replay-playing');

    await page.reload({waitUntil: 'domcontentloaded'});
    await boot();
    const after = await page.evaluate(() => ({
      index: JSON.parse(localStorage.getItem('th13.replays') || '[]'),
      blob: localStorage.getItem('th13.replay.th13_01.rpy'),
      scoredat: localStorage.getItem('th13.scoredat'),
      hint: window.mod._th13_replay_import_hint(),
      failures: (window.__th13_log ?? []).filter(l => l.startsWith('replay-restore') || l.startsWith('replay-import-error')),
      probe: window.__e2e.probe(),
    }));
    const entryBefore = saved.index.find(e => e.name === 'th13_01.rpy');
    const entryAfter = after.index.find(e => e.name === 'th13_01.rpy');
    const persisted = !!after.blob && after.blob === saved.blob &&
                      !!entryAfter && entryAfter.size === (entryBefore ?? {}).size;
    mark('reload-persistence', {persisted, entryAfter, hint: after.hint,
                                failures: after.failures});
    if (!persisted) throw Error('replay did not survive page reload');
    if (after.scoredat !== saved.scoredat) throw Error('scoredat changed across reload');
    if (JSON.stringify(after.failures) !== '[]')
      throw Error('restore failures after reload: ' + JSON.stringify(after.failures));

    await writeFile(join(outDir, 'e2e_report.json'), JSON.stringify({
      mode: 'full', bootSeed, stage1Seed, wasmSha256: build.sha256,
      inputs: {source: inputs.source, player_name: inputs.player_name,
               score: inputs.score, character: inputs.character, rank: inputs.rank,
               stages: inputs.stages.map(s =>
                 ({number: s.number, seed: s.seed, frames: s.frames, start_score: s.start_score}))},
      exportBytes: blobBytes.length, hintAfterSave: saved.hint,
      persisted: true, timeline,
      consoleTail: consoleLines.slice(-120), pageErrors,
      elapsedMin: +((Date.now() - started) / 60000).toFixed(2),
    }, null, 2) + '\n');
    mark('full-run-done');
  }
} catch (error) {
  mark('failure', {error: String((error && error.stack) ?? error)});
  try { await page?.screenshot({path: join(outDir, 'failure.png'), timeout: 15000}); } catch {}
  await writeFile(join(outDir, 'e2e_report.json'), JSON.stringify({
    mode: limitStage > 0 ? 'smoke' : 'full', bootSeed, timeline,
    consoleTail: consoleLines.slice(-200), pageErrors,
    failure: String((error && error.stack) ?? error),
  }, null, 2) + '\n').catch(() => {});
  throw error;
} finally {
  await browser?.close().catch(() => {});
  await new Promise(done => server.close(done));
}
