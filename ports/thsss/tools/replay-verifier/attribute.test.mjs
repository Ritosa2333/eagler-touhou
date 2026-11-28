import assert from 'node:assert/strict';
import {mkdtempSync, readFileSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {dirname, resolve} from 'node:path';
import {test} from 'node:test';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {attribute} from './attribute.mjs';
import {framesOf, writeTrace} from './observation.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const common = resolve(process.env.EAGLER_COMMON_ROOT || resolve(here, '../../../../common'));
const {compareTraces} = await import(pathToFileURL(resolve(common, 'testkit/replay-verifier/compare.mjs')));
const {readTraceFile} = await import(pathToFileURL(resolve(common, 'testkit/replay-verifier/golden-files.mjs')));

const identity = {
  game: 'thsss',
  profile: 'retail-1.01/post-mainmenu-replay-v1',
  replaySha256: 'b'.repeat(64),
  executableSha256: 'c'.repeat(64),
  resourceSha256: 'd'.repeat(64),
  stateSchema: 'thsss/authoritative-state/v1',
  traceCodec: 'jsonl/v1',
  digestAlgorithm: 'sha256-truncated-128/canonical-json-v1',
};

function row(name, rngShape = 'framework') {
  const categories = {
    clocks: {timeMain: 1, timeTotal: '1', realTimeTotal: '1', stageId: 1, difficulty: 0, stageName: name},
    input: {held: 0},
    rng: {ranValue: 1, shape: rngShape, inext: 0, inextp: 21, seed: [1]},
    player: {x: 1, y: 2, life: 2, score: '0'},
    economy: {continueTimes: 0, slowMode: 0, clearCheck: '0'},
    boss: {present: 0},
    entities: {bullets: [], myBullets: [], enemies: [], items: [], spells: []},
    draw: {effects: 3},
  };
  return {
    record: true,
    stageName: name,
    stageId: 1,
    appliedInput: 0,
    clocks: categories.clocks,
    scalars: {stageId: 1, difficulty: 0, stageName: name},
    categories,
  };
}

test('splash frames stay outside the required trace', () => {
  const rows = [{record: false, stageName: 'Splash'}, row('St1'), {record: false, ended: true, complete: true, reason: 'replay-complete'}];
  assert.equal(framesOf(rows).length, 1);
  const dir = mkdtempSync(resolve(tmpdir(), 'thsss-verify-'));
  const path = resolve(dir, 'sample.jsonl');
  const written = writeTrace(path, {identity, provider: 'thsss/retail-proton-inject', provenance: {}, rows});
  assert.equal(written.ticks, 1);
  const text = readFileSync(path, 'utf8');
  assert.equal(text.includes('Splash'), false);
  assert.match(text, /post-mainmenu-replay/);
});

test('a known player-field change is reported as a web divergence', async () => {
  const dir = mkdtempSync(resolve(tmpdir(), 'thsss-verify-'));
  const expected = resolve(dir, 'expected.jsonl');
  const actual = resolve(dir, 'actual.jsonl');
  const rows = [row('St1'), {record: false, ended: true, complete: true, reason: 'replay-complete'}];
  writeTrace(expected, {identity, provider: 'thsss/retail-proton-inject', provenance: {}, rows});
  writeTrace(actual, {identity, provider: 'thsss/diagnostic-wasm', provenance: {}, rows});
  const lines = readFileSync(actual, 'utf8').split('\n');
  const tick = JSON.parse(lines.find(line => line.includes('"type":"tick"')));
  tick.categories.player = '0'.repeat(32);
  lines[lines.findIndex(line => line.includes('"type":"tick"'))] = JSON.stringify(tick);
  writeFileSync(actual, lines.join('\n'));
  const result = await compareTraces(readTraceFile(expected), readTraceFile(actual));
  const judged = attribute(result, framesOf(rows), framesOf(rows));
  assert.equal(result.status, 'DIVERGED');
  assert.equal(result.reason, 'category-digest');
  assert.equal(result.firstDifference.category, 'player');
  assert.equal(judged.attributionHint, 'web');
});

test('framework versus core rng is not called a capture error', () => {
  const expected = [row('St1', 'framework')];
  const actual = [row('St1', 'core')];
  const judged = attribute({status: 'DIVERGED', reason: 'category-digest', comparedTicks: 0, firstDifference: {category: 'rng'}}, expected, actual);
  assert.equal(judged.attributionHint, 'rng-runtime');
  assert.equal(judged.fieldDiff.path, 'rng.shape');
});
