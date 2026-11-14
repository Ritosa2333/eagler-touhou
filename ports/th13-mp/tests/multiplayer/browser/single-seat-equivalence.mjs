// Real SP build versus real MP build, using the same original replay input.
// Compare every field exported by each shared read-only state projection.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {createRequire} from 'node:module';
import {readFile, writeFile, appendFile, mkdir} from 'node:fs/promises';
import {resolve, extname, sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {gzipSync} from 'node:zlib';

const workspace = fileURLToPath(new URL('../../../../../../', import.meta.url));
const repository = resolve(workspace, 'eagler-touhou');
const output = resolve(process.env.TH13_EQ_ARTIFACTS ||
  resolve(workspace, 'artifacts/mp-sync-20261112/single-seat-browser'),
  process.env.TH13_EQ_CASE || 'four-characters');
const limit = Number(process.env.TH13_EQ_FRAMES || 7200);
assert(Number.isSafeInteger(limit) && limit > 0, 'Positive frame limit');
const stageIndex = Number(process.env.TH13_EQ_STAGE || 0);
assert(Number.isSafeInteger(stageIndex) && stageIndex >= 0, 'Nonnegative replay stage index');
const bootSeed = 47932;
const cases = [
  {id: 'character-0', character: 0, replay: 'th13_01.rpy'},
  {id: 'character-1', character: 1, replay: 'th13_udMy25.rpy'},
  {id: 'character-2', character: 2, replay: 'th13_udSr01.rpy'},
  {id: 'character-3', character: 3, replay: 'th13_udN01.rpy'},
  // Extra replay contains an actual X press within the fixed window. Its
  // header, difficulty, seed and all six input bytes still come from the file.
  {id: 'character-2-extra', character: 2, replay: 'th13_udWi11.rpy', requiresBomb: true},
].filter(value => process.env.TH13_EQ_CHARACTER === undefined ||
  value.character === Number(process.env.TH13_EQ_CHARACTER));
assert(cases.length > 0, 'Requested character exists');
await mkdir(resolve(output, '..'), {recursive: true});
await mkdir(output); // Never append a rerun to old evidence.
await writeFile(resolve(output, 'single-seat-equivalence.mjs'), await readFile(fileURLToPath(import.meta.url)));
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const report = {started: new Date().toISOString(), passed: false,
  scope: 'Actual SP and MP single-seat replay browsers; complete shared exported values, not complete internal world or framebuffer certification',
  bootSeed, stageIndex, frameLimit: limit, builds: {}, cases: [], errors: [],
  unassessed: ['Internal state not present in the common read-only exports', 'Independent original-game fidelity',
    'Final framebuffer equality', 'Multiplayer network quality and full room flow']};
const builds = {};
for (const variant of ['sp', 'mp']) {
  const port = resolve(repository, 'ports', variant === 'sp' ? 'th13' : 'th13-mp');
  const directory = resolve(process.env[variant === 'sp' ? 'TH13_EQ_SP_BUILD' : 'TH13_EQ_MP_BUILD'] ||
    resolve(port, variant === 'sp' ? 'build-eagler' : 'build-eagler-multiplayer'));
  const files = {};
  for (const name of ['th13-sdl.mjs', 'th13-sdl.wasm']) files[name] = await readFile(resolve(directory, name));
  builds[variant] = {port, directory, files};
  report.builds[variant] = {directory, files: Object.entries(files).map(([name, bytes]) =>
    ({name, bytes: bytes.length, sha256: hash(bytes)}))};
}
assert.notEqual(report.builds.sp.files[1].sha256, report.builds.mp.files[1].sha256,
  'SP and MP must be separate actual builds');
const mime = {'.html': 'text/html', '.mjs': 'text/javascript', '.js': 'text/javascript',
  '.wasm': 'application/wasm', '.json': 'application/json', '.png': 'image/png'};
const server = createServer(async (request, response) => {
  try {
    const path = decodeURIComponent(new URL(request.url, 'http://localhost').pathname);
    const match = /^\/(sp|mp)\/(.*)$/.exec(path);
    if (!match) throw Error('Unknown variant');
    const [, variant, rest] = match, build = builds[variant];
    let root, relative;
    if (rest.startsWith('build/')) {
      const bytes = build.files[rest.slice(6)];
      if (!bytes) throw Error('Unknown executable');
      response.writeHead(200, {'Content-Type': mime[extname(rest)], 'Cache-Control': 'no-store'}).end(bytes);
      return;
    } else if (rest.startsWith('assets/') && rest.endsWith('.sht')) {
      root = resolve(workspace, 'artifacts/reference/raw'); relative = rest.slice(7);
    } else if (rest.startsWith('assets/')) {
      root = resolve(build.port, 'web/sdl-runtime/assets'); relative = rest.slice(7);
    } else if (rest.startsWith('replays/')) {
      root = resolve(workspace, 'games/replays'); relative = rest.slice(8);
    } else { root = resolve(build.port, 'web/sdl-runtime'); relative = rest; }
    const file = resolve(root, relative);
    if (!file.startsWith(root + sep)) throw Error('Path escape');
    const bytes = await readFile(file);
    response.writeHead(200, {'Content-Type': mime[extname(file)] || 'application/octet-stream'}).end(bytes);
  } catch { response.writeHead(404).end('not found'); }
});
await new Promise(done => server.listen(0, '127.0.0.1', done));
const origin = `http://127.0.0.1:${server.address().port}`;
const require = createRequire(import.meta.url);
const {chromium} = require(process.env.PLAYWRIGHT_MODULE || '/opt/shot/node_modules/playwright-core');
const angle = process.env.TH13_ANGLE || 'gl-egl';
assert(['gl-egl', 'gl', 'swiftshader'].includes(angle), 'Supported ANGLE backend');
const browserArgs = [
  '--no-sandbox', '--disable-dev-shm-usage', '--ignore-gpu-blocklist', '--disable-gpu-sandbox',
  `--use-angle=${angle}`, ...(angle === 'swiftshader' ? ['--enable-unsafe-swiftshader'] : ['--disable-software-rasterizer']),
  '--autoplay-policy=no-user-gesture-required'];
const browser = await chromium.launch({headless: process.env.TH13_HEADFUL !== '1', channel: 'chromium', args: browserArgs});
report.browser = {version: browser.version(), channel: 'chromium', headless: process.env.TH13_HEADFUL !== '1',
  args: browserArgs, angle};
const exports = ['_th13_debug_replay_progress', '_th13_debug_replay_core',
  '_th13_debug_replay_entities', '_th13_debug_anm_state'];
const projectionNames = ['progress', 'core', 'entities', 'anm'];
const replayDecoder = `import sys,json,pathlib,hashlib,base64
sys.path.insert(0,sys.argv[1])
from oracle.replayfmt import ReplayFile
p=pathlib.Path(sys.argv[2]);b=p.read_bytes();r=ReplayFile();r.decode(b);i=int(sys.argv[3]);s=r.stages[i]
print(json.dumps(dict(file=str(p),sha256=hashlib.sha256(b).hexdigest(),bytes=len(b),character=r.character,rank=r.rank,
 stageIndex=i,stage=s.number,seed=s.stamp,header=base64.b64encode(s.header).decode(),frames=s.frame_count,
 inputSha256=hashlib.sha256(s.input).hexdigest(),input=base64.b64encode(s.input).decode())))`;
const snapshot = ({steps, exports}) => {
  const m = window.mod, result = [];
  const read = name => { const pointer = m[name](); if (!pointer) throw Error('Missing state ' + name); return m.UTF8ToString(pointer); };
  for (let i = 0; i < steps; ++i) {
    if (!m._th13_debug_frame()) throw Error(m.UTF8ToString(m._th13_error()));
    result.push(exports.map(read));
  }
  return result;
};
try {
  for (const spec of cases) {
    const raw = spawnSync('python3', ['-c', replayDecoder, workspace,
      resolve(workspace, 'games/replays', spec.replay), String(stageIndex)], {encoding: 'utf8'});
    assert.equal(raw.status, 0, raw.stderr);
    const replay = JSON.parse(raw.stdout), inputs = Buffer.from(replay.input, 'base64');
    assert.equal(replay.character, spec.character, 'Actual replay character');
    assert.equal(inputs.length, replay.frames * 6, 'Complete six-byte input history');
    await writeFile(resolve(output, `${spec.id}-input.bin`), inputs);
    const row = {...spec, replay: {...replay, input: undefined}, comparedFrames: 0,
      projections: projectionNames, delivery: [], coverage: {bombFrames: 0, dialogueFrames: 0, bombPressFrames: [], firstBombFrame: null}, passed: false};
    report.cases.push(row);
    const clients = [], deliveries = [];
    for (const variant of ['sp', 'mp']) {
      const context = await browser.newContext({viewport: {width: 640, height: 520}});
      await context.addInitScript(seed => { window.__th13_boot_seed = seed; }, bootSeed);
      const page = await context.newPage(), logs = [];
      page.on('pageerror', error => report.errors.push({variant, character: spec.character, message: error.message, stack: error.stack}));
      page.on('console', message => logs.push({type: message.type(), text: message.text()}));
      page.on('response', response => {
        if (!/\/build\/th13-sdl\.(mjs|wasm)$/.test(new URL(response.url()).pathname)) return;
        deliveries.push((async () => { const bytes = await response.body();
          row.delivery.push({variant, url: response.url(), bytes: bytes.length, sha256: hash(bytes)}); })());
      });
      await page.goto(origin + `/${variant}/th13-debug.html?artifact=/${variant}/build/th13-sdl.mjs&` +
        `replay=${spec.replay}&sht=pl0${spec.character}a.sht&stage=${stageIndex}`);
      await page.waitForFunction(() => !!window.__th13_debug || !!window.__th13_debug_failed, null, {timeout: 120000});
      assert.equal(await page.evaluate(() => window.__th13_debug_failed || null), null, 'Real replay page boot');
      const gpu = await page.evaluate(() => {
        const gl = document.getElementById('canvas').getContext('webgl2');
        if (!gl) throw Error('Production canvas has no WebGL2 context');
        const debug = gl.getExtension('WEBGL_debug_renderer_info');
        return {version: gl.getParameter(gl.VERSION), vendor: gl.getParameter(gl.VENDOR),
          renderer: gl.getParameter(gl.RENDERER),
          unmaskedVendor: debug ? gl.getParameter(debug.UNMASKED_VENDOR_WEBGL) : null,
          unmaskedRenderer: debug ? gl.getParameter(debug.UNMASKED_RENDERER_WEBGL) : null};
      });
      (row.gpu ??= {})[variant] = gpu;
      const ready = await page.evaluate(names => ({seed: window.mod._th13_boot_seed(),
        exports: names.map(name => [name, typeof window.mod[name]]), total: window.__th13_debug.total,
        initial: names.map(name => window.mod.UTF8ToString(window.mod[name]()))}), exports);
      assert.equal(ready.seed, bootSeed, 'Actual boot seed');
      assert.equal(ready.total, replay.frames, 'Actual replay total');
      assert(ready.exports.every(([, type]) => type === 'function'), 'All common exports must exist');
      clients.push({variant, context, page, logs, initial: ready.initial});
    }
    await writeFile(resolve(output, `${spec.id}-initial.json`), JSON.stringify({
      sp: clients[0].initial.map(JSON.parse), mp: clients[1].initial.map(JSON.parse)}, null, 2));
    assert.deepEqual(clients[0].initial, clients[1].initial, 'All shared pre-input projections');
    for (const client of clients)
      assert(client.logs.some(log => log.text.includes(`R3-RESTORE stage=${replay.stage} seed=${replay.seed} counter=0`)),
        'Actual production restore identifies the replay stage seed and consumption phase');
    const count = Math.min(limit, replay.frames);
    for (let first = 0; first < count; first += 30) {
      const steps = Math.min(30, count - first);
      const batches = await Promise.all(clients.map(client => client.page.evaluate(snapshot, {steps, exports})));
      for (let variant = 0; variant < 2; ++variant)
        await appendFile(resolve(output, `${spec.id}-${clients[variant].variant}.ndjson.gz`),
          gzipSync(batches[variant].map((values, offset) => JSON.stringify({frame: first + offset,
            values: Object.fromEntries(projectionNames.map((name, index) => [name, values[index]]))}) + '\n').join(''), {level: 1}));
      for (let offset = 0; offset < steps; ++offset) {
        const frame = first + offset;
        try {
          for (let projection = 0; projection < exports.length; ++projection)
            assert.equal(batches[0][offset][projection], batches[1][offset][projection],
              `Character ${spec.character} frame ${frame} full ${projectionNames[projection]}`);
          if (frame === 0) {
            row.negativeControls = [];
            for (let projection = 0; projection < exports.length; ++projection) {
              const changed = JSON.parse(batches[1][offset][projection]);
              if (projection === 0) changed.consumed++;
              if (projection === 1) changed.globals['004be7b8'] ^= 1;
              if (projection === 2) changed.frame++;
              if (projection === 3) changed.vms[0].flags ^= 1;
              assert.throws(() => assert.equal(batches[0][offset][projection], JSON.stringify(changed)),
                undefined, projectionNames[projection] + ' mutation must be rejected');
              row.negativeControls.push({projection: projectionNames[projection], rejected: true});
            }
          }
          const [progress, core, entities, anm] = batches[0][offset].map(JSON.parse);
          assert.equal(progress.consumed, frame + 1, 'Exactly one replay record consumed');
          assert.equal(progress.slot_frame, frame + 1, 'Same stage cursor consumption phase');
          assert.equal(core.frame, frame, 'Last consumed frame identity');
          assert.equal(core.globals['004be7b8'], spec.character, 'Actual character remains selected');
          const sixBytes = [0, 2, 4].map(at => inputs.readUInt16LE(frame * 6 + at));
          assert.deepEqual([core.input.held, core.input.pressed, core.input.released], sixBytes,
            'All six recorded input bytes consumed in their original phase');
          if (core.input.pressed & 2) row.coverage.bombPressFrames.push(frame);
          if (entities.timers.bomb > 0) {
            ++row.coverage.bombFrames;
            if (row.coverage.firstBombFrame === null) row.coverage.firstBombFrame = frame;
          }
          if (anm.dialogue.active) ++row.coverage.dialogueFrames;
          ++row.comparedFrames;
        } catch (error) {
          await writeFile(resolve(output, `${spec.id}-first-difference.json`),
            JSON.stringify({frame, reason: error.message, sp: batches[0][offset].map(JSON.parse),
              mp: batches[1][offset].map(JSON.parse)}, null, 2));
          throw error;
        }
      }
      if (first % 600 === 0) {
        console.log(JSON.stringify({character: spec.character, compared: row.comparedFrames, coverage: row.coverage}));
        await writeFile(resolve(output, 'progress.json'), JSON.stringify(report, null, 2));
      }
    }
    await Promise.all(deliveries);
    for (const variant of ['sp', 'mp']) for (const expected of report.builds[variant].files) {
      const delivered = row.delivery.filter(item => item.variant === variant && item.url.endsWith('/' + expected.name));
      assert.equal(delivered.length, 1, 'Each actual executable response is observed once');
      assert.equal(delivered[0].bytes, expected.bytes, 'Executable response length');
      assert.equal(delivered[0].sha256, expected.sha256, 'Executable response identity');
    }
    for (const client of clients) {
      await writeFile(resolve(output, `${spec.id}-${client.variant}-console.json`), JSON.stringify(client.logs));
      await client.context.close();
    }
    row.passed = true;
  }
  assert.equal(report.errors.length, 0, 'No browser errors');
  report.projectionComparisonPassed = report.cases.every(row => row.passed);
  report.requiredCoverage = {fourCharacters: [0,1,2,3].every(character => report.cases.some(row => row.character === character)),
    allCasesFullWindow: report.cases.every(row => row.comparedFrames === limit),
    extraReplayBomb: report.cases.some(row => row.requiresBomb && row.coverage.bombFrames > 0 && row.coverage.bombPressFrames.length > 0),
    bomb: report.cases.some(row => row.coverage.bombFrames > 0),
    dialogue: report.cases.some(row => row.coverage.dialogueFrames > 0)};
  report.passed = report.projectionComparisonPassed && Object.values(report.requiredCoverage).every(Boolean);
} catch (error) { report.failure = error.stack; }
finally {
  report.finished = new Date().toISOString();
  await writeFile(resolve(output, 'report.json'), JSON.stringify(report, null, 2) + '\n');
  await browser.close();
  await new Promise(done => server.close(done));
}
console.log(JSON.stringify({passed: report.passed, failure: report.failure, coverage: report.requiredCoverage,
  cases: report.cases.map(row => ({character: row.character, frames: row.comparedFrames, coverage: row.coverage}))}));
if (!report.passed) process.exitCode = 1;
