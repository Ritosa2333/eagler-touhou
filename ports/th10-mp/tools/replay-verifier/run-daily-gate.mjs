#!/usr/bin/env node
import {readFile, writeFile} from 'node:fs/promises';
import {dirname, isAbsolute, join, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {recordsFromLongJsonLines} from './adapter.mjs';
import {assetPath, loadGoldenManifest, readJsonLinesMaybeGzip} from './golden.mjs';

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
if (!args['capture-root']) throw Error('usage: node run-daily-gate.mjs --capture-root <lunatic-and-extra-directory> [--golden-root <published-golden>] [--common-root <eagler-common>] [--report <file>]');
const captureRoot = resolve(args['capture-root']);
const commonRoot = resolve(args['common-root'] ?? process.env.EAGLER_COMMON_ROOT ?? join(here, '..', '..', '..', 'eagler-common'));
const {compareTraces} = await import(pathToFileURL(join(commonRoot, 'testkit', 'replay-verifier', 'compare.mjs')));
const common = await import(pathToFileURL(join(commonRoot, 'testkit', 'replay-verifier', 'adapter.mjs')));
const golden = await loadGoldenManifest(args['golden-root']);
const results = [];
for (const id of golden.manifest.suites.daily) {
  const asset = golden.manifest.assets[id];
  try {
    const metadataPath = join(captureRoot, `${id}.json`);
    const candidate = JSON.parse(await readFile(metadataPath, 'utf8'));
    if (!candidate.complete || candidate.replay?.sha256 !== asset.replaySha256)
      throw Error(`${id}: candidate capture is incomplete or uses a different Replay`);
    const candidateRows = isAbsolute(candidate.rows.path) ? candidate.rows.path : resolve(dirname(metadataPath), candidate.rows.path);
    const fixture = {replaySha256: asset.replaySha256};
    const result = await compareTraces(
      recordsFromLongJsonLines({fixture, rows: readJsonLinesMaybeGzip(assetPath(golden.root, asset)), provider: 'th10/published-golden-v1', identity: golden.manifest.identity, complete: true}, common),
      recordsFromLongJsonLines({fixture, rows: readJsonLinesMaybeGzip(candidateRows), provider: candidate.provider ?? 'th10-eagler/current-candidate', identity: golden.manifest.identity, complete: true}, common),
    );
    results.push({id, expectedTicks: asset.comparedTicks, ...result});
  } catch (error) {
    results.push({id, schema: 'eagler/replay-result/v1', status: 'ERROR', reason: 'adapter-error', message: error.message});
  }
}
const report = {schema: 'th10/replay-verifier-suite-result/v1', suite: 'daily', results, passed: results.every(result => result.status === 'PASS')};
if (args.report) await writeFile(resolve(args.report), `${JSON.stringify(report, null, 2)}\n`);
for (const result of results) process.stdout.write(`${result.id}: ${result.status} (${result.comparedTicks ?? 0} ticks)${result.message ? ` - ${result.message}` : ''}\n`);
process.exitCode = report.passed ? 0 : 1;
