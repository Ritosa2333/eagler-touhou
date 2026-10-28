// Strict gate: never writes golden. BASELINE_HELD is not a fidelity pass.
import {spawnSync} from 'node:child_process';
import {readFileSync, existsSync, createReadStream} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {parseArgs} from 'node:util';
import {parseComparison, checkIdentity, classify} from './gate-protocol.mjs';

const {values} = parseArgs({options: {
  case: {type: 'string'}, lane: {type: 'string', default: 'all'},
  'candidate-root': {type: 'string'}, golden: {type: 'string'},
  'accept-golden': {type: 'boolean'},
}, strict: true, allowPositionals: false});
if (values['accept-golden']) throw Error('--accept-golden is disabled. Migrate golden in a separate audited operation: preserve corpus/replay identity, record corpusSha256/oracleSha256/caseId/replaySha256 under identity, verify explicit window and monotonic frontier. Ordinary gate never writes golden.');
const here = fileURLToPath(new URL('./', import.meta.url));
const corpusBytes = readFileSync(resolve(here, 'corpus.json'));
const corpus = JSON.parse(corpusBytes);
const corpusSha256 = createHash('sha256').update(corpusBytes).digest('hex');
const ids = values.case ? [values.case] : values.lane === 'all'
  ? [...new Set(Object.values(corpus.suites).flat())] : corpus.suites[values.lane];
if (!ids?.length) throw Error('Unknown/empty suite');
const candidateRoot = values['candidate-root'] || resolve(here, '../../artifacts/replay-verifier');
const goldenPath = values.golden || resolve(here, 'golden/baseline.json');
const golden = existsSync(goldenPath) ? JSON.parse(readFileSync(goldenPath, 'utf8')) : {cases: {}};
const hashFile = async path => {
  const hash = createHash('sha256');
  for await (const chunk of createReadStream(path)) hash.update(chunk);
  return hash.digest('hex');
};
const results = [];
for (const id of ids) {
  const fixture = corpus.cases.find(c => c.id === id);
  if (!fixture) throw Error('Unknown case ' + id);
  const prior = golden.cases[id];
  // A historical observed baseline length is not independent window qualification.
  const expected = fixture.frames;
  if (!Number.isSafeInteger(expected) || expected <= 0) throw Error('Explicit corpus frames required; legacy baseline window is unqualified: ' + id);
  const oracle = resolve('/work/artifacts/oracle-draws', fixture.oracleManifest);
  checkIdentity(prior, fixture, {corpusSha256, oracleSha256: await hashFile(oracle)});
  if (prior && prior.window !== expected) throw Error('Golden/corpus window mismatch');
  const candidate = resolve(candidateRoot, id + '.candidate.jsonl');
  const run = spawnSync('python3', ['-m', 'oracle.draw_compare', candidate, oracle,
    '--expected-frames', String(expected)], {cwd: '/work', encoding: 'utf8'});
  const verdict = parseComparison(run, expected);
  results.push({id, first_divergence_frame: verdict.first, window: verdict.window,
    ops: verdict.ops, status: classify(verdict, prior), line: verdict.line});
}
const status = results.every(r => r.status === 'EXACT_MATCH') ? 'EXACT_MATCH'
  : results.every(r => ['EXACT_MATCH', 'BASELINE_HELD'].includes(r.status)) ? 'BASELINE_HELD' : 'FAIL';
console.log(JSON.stringify({schema: 'th13/replay-draws-gate/v2', lane: values.lane, status, results}, null, 2));
process.exitCode = status === 'EXACT_MATCH' ? 0 : 1;
