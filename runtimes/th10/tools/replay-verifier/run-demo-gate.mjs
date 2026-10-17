#!/usr/bin/env node
import {readFile, writeFile} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {recordsFromRows} from './adapter.mjs';
import {assetPath, loadGoldenManifest, readJsonMaybeGzip} from './golden.mjs';

const here = dirname(fileURLToPath(import.meta.url));
function options(argv) {
  const result = {};
  for (let index = 0; index < argv.length; index++) {
    const token = argv[index];
    if (!token.startsWith('--') || index + 1 >= argv.length) throw Error(`Invalid argument ${token}`);
    result[token.slice(2)] = argv[++index];
  }
  return result;
}
const args = options(process.argv.slice(2));
if (!args.candidate) throw Error('usage: node run-demo-gate.mjs --candidate <suite.json> [--golden-root <published-golden>] [--original <advanced-source-suite>] [--common-root <eagler-common>] [--report <file>]');
const commonRoot = resolve(args['common-root'] ?? join(here, '..', '..', '..', 'eagler-common'));
const {compareTraces} = await import(pathToFileURL(join(commonRoot, 'testkit/replay-verifier/compare.mjs')));
const commonAdapter = await import(pathToFileURL(join(commonRoot, 'testkit/replay-verifier/adapter.mjs')));
const golden = args.original ? null : await loadGoldenManifest(args['golden-root']);
const [corpus, original, candidate] = await Promise.all([
  readFile(join(here, 'corpus.json'), 'utf8').then(JSON.parse),
  args.original ? readFile(resolve(args.original), 'utf8').then(JSON.parse) : readJsonMaybeGzip(assetPath(golden.root, golden.manifest.assets.demos)),
  readFile(resolve(args.candidate), 'utf8').then(JSON.parse),
]);
const results = [];
if (!original.complete || original.schema !== 'th10/original-title-demo-capture/v1') throw Error('Original capture is incomplete or invalid');
if (!candidate.complete || candidate.schema !== 'th10/current-title-demo-capture/v1' || candidate.errors?.length) throw Error('Candidate capture is incomplete or invalid');
const fixtures = corpus.fixtures;
if (original.demos.length !== fixtures.length || candidate.demos.length !== fixtures.length) throw Error('Demo rotation coverage is incomplete');
for (let index = 0; index < fixtures.length; index++) {
  const fixture = fixtures[index], expected = original.demos[index], actual = candidate.demos[index];
  if (expected.rotationIndex !== index || actual.rotationIndex !== index || expected.rows.length !== fixture.expectedTicks || actual.rows.length !== fixture.expectedTicks) {
    results.push({id: fixture.id, schema: 'eagler/replay-result/v1', status: 'INCOMPLETE', reason: 'rotation-or-length', expectedTicks: fixture.expectedTicks, originalTicks: expected.rows.length, candidateTicks: actual.rows.length});
    continue;
  }
  const comparison = await compareTraces(
    recordsFromRows(fixture, expected.rows, 'th10-original-jit/present-observer', commonAdapter),
    recordsFromRows(fixture, actual.rows, `th10-eagler/diagnostic-browser/${candidate.build?.wasm ?? 'unknown'}`, commonAdapter),
  );
  results.push({id: fixture.id, rotationIndex: index, currentBuild: candidate.build?.wasm, ...comparison});
}
const report = {schema: 'th10/replay-verifier-suite-result/v1', suite: 'title-demo', source: args.original ? 'advanced-original-capture' : 'published-golden', results, passed: results.length === fixtures.length && results.every(result => result.status === 'PASS')};
if (args.report) await writeFile(resolve(args.report), `${JSON.stringify(report, null, 2)}\n`);
for (const result of results) process.stdout.write(`${result.id}: ${result.status} (${result.comparedTicks ?? 0} ticks)\n`);
process.exitCode = report.passed ? 0 : 1;
