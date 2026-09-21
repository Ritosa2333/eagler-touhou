#!/usr/bin/env node
import {readFile, writeFile} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {loadLegacyOriginal, recordsFromRows} from './legacy-oracle-adapter.mjs';
import {assetPath, loadGoldenManifest, readJsonLinesArray} from './golden.mjs';

const here = dirname(fileURLToPath(import.meta.url));
function options(argv) {
  const result = {};
  for (let index = 0; index < argv.length; index++) {
    const token = argv[index];
    if (!token.startsWith('--') || index + 1 >= argv.length) throw new Error(`Invalid argument ${token}`);
    result[token.slice(2)] = argv[++index];
  }
  return result;
}

const args = options(process.argv.slice(2));
if (!args['capture-root'])
  throw new Error('usage: node run-current-candidate-gate.mjs --capture-root <capture-directory> [--golden-root <published-golden>] [--oracle-root <advanced-source>] [--common-root <eagler-common>] [--report <file>]');
const oracleRoot = args['oracle-root'] ? resolve(args['oracle-root']) : null, captureRoot = resolve(args['capture-root']);
const commonRoot = resolve(args['common-root'] ?? process.env.EAGLER_COMMON_ROOT ?? join(here, '..', '..', '..', 'eagler-common'));
const {compareTraces} = await import(pathToFileURL(join(commonRoot, 'testkit', 'replay-verifier', 'compare.mjs')));
const commonAdapter = await import(pathToFileURL(join(commonRoot, 'testkit', 'replay-verifier', 'adapter.mjs')));
const corpus = JSON.parse(await readFile(join(here, 'corpus.json'), 'utf8'));
const golden = oracleRoot ? null : await loadGoldenManifest(args['golden-root']);
const results = [];
for (const fixture of corpus.fixtures) {
  try {
    const [original, capture] = await Promise.all([
      oracleRoot
        ? loadLegacyOriginal(oracleRoot, fixture)
        : readJsonLinesArray(assetPath(golden.root, golden.manifest.assets[fixture.id])).then(rows => ({rows})),
      readFile(join(captureRoot, `${fixture.id}.json`), 'utf8').then(JSON.parse),
    ]);
    if (capture.schema !== 'th08/current-replay-capture/v1' || capture.fixture !== Number(fixture.id.slice(4)) || !capture.complete || capture.errors?.length)
      throw new Error(`${fixture.id}: current capture is incomplete or invalid`);
    const comparison = await compareTraces(
      recordsFromRows(fixture, original.rows, 'th08-original-jit/historical-artifact', commonAdapter),
      recordsFromRows(fixture, capture.rows, `th08-eagler/current-browser/${capture.build?.wasm ?? 'unknown'}`, commonAdapter),
    );
    results.push({id: fixture.id, currentBuild: capture.build?.wasm, rawCapturedTicks: capture.rows.length, ...comparison});
  } catch (error) {
    results.push({id: fixture.id, schema: 'eagler/replay-result/v1', status: 'ERROR', reason: 'adapter-error', message: error.message});
  }
}
const report = {
  schema: 'th08/replay-verifier-suite-result/v1', suite: corpus.suite,
  scope: `${oracleRoot ? 'Advanced original-JIT source' : 'Published immutable golden'} versus the named current th08-eagler diagnostic Browser Runtime build; limited legacy field coverage.`,
  results, passed: results.length === corpus.fixtures.length && results.every(result => result.status === 'PASS'),
};
if (args.report) await writeFile(resolve(args.report), `${JSON.stringify(report, null, 2)}\n`);
for (const result of results) process.stdout.write(`${result.id}: ${result.status} (${result.comparedTicks ?? 0} ticks)${result.message ? ` - ${result.message}` : ''}\n`);
process.exitCode = report.passed ? 0 : 1;
