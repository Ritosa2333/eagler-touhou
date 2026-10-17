#!/usr/bin/env node
import {readFile, writeFile} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {loadLegacyOraclePair} from './legacy-oracle-adapter.mjs';

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
if (!args['oracle-root']) throw new Error('usage: node run-legacy-oracle-gate.mjs --oracle-root <TouhouDev/th08_web> [--common-root <eagler-common>] [--report <file>]');
const oracleRoot = resolve(args['oracle-root']);
const commonRoot = resolve(args['common-root'] ?? process.env.EAGLER_COMMON_ROOT ?? join(here, '..', '..', '..', 'eagler-common'));
const {compareTraces} = await import(pathToFileURL(join(commonRoot, 'testkit', 'replay-verifier', 'compare.mjs')));
const commonAdapter = await import(pathToFileURL(join(commonRoot, 'testkit', 'replay-verifier', 'adapter.mjs')));
const corpus = JSON.parse(await readFile(join(here, 'corpus.json'), 'utf8'));
const results = [];
for (const fixture of corpus.fixtures) {
  try {
    const pair = await loadLegacyOraclePair(oracleRoot, fixture, commonAdapter);
    results.push({id: fixture.id, ...(await compareTraces(pair.expected, pair.actual))});
  } catch (error) {
    results.push({id: fixture.id, schema: 'eagler/replay-result/v1', status: 'ERROR', reason: 'adapter-error', message: error.message});
  }
}
const report = {
  schema: 'th08/replay-verifier-suite-result/v1', suite: corpus.suite,
  scope: 'Historical TH08 original-JIT versus reconstruction artifacts; this does not test the current th08-eagler runtime.',
  results,
  passed: results.length === corpus.fixtures.length && results.every(result => result.status === 'PASS'),
};
if (args.report) await writeFile(resolve(args.report), `${JSON.stringify(report, null, 2)}\n`);
for (const result of results) process.stdout.write(`${result.id}: ${result.status} (${result.comparedTicks ?? 0} ticks)${result.message ? ` - ${result.message}` : ''}\n`);
process.exitCode = report.passed ? 0 : 1;
