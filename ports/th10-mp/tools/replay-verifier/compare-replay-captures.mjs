#!/usr/bin/env node
import {createHash} from 'node:crypto';
import {createReadStream, readFileSync, writeFileSync} from 'node:fs';
import {createInterface} from 'node:readline';
import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {recordsFromLongJsonLines} from './adapter.mjs';

const option = name => { const index = process.argv.indexOf(name); return index < 0 ? undefined : process.argv[index + 1]; };
const expectedPath = resolve(option('--expected') ?? ''), actualPath = resolve(option('--actual') ?? '');
const oracleRoot = resolve(option('--oracle-root') ?? 'D:/workspace/东方测试.zip/TouhouDev/th10_web');
const output = option('--output') ? resolve(option('--output')) : null;
const maxTicks = option('--max-ticks') ? Number(option('--max-ticks')) : null;
if (!option('--expected') || !option('--actual')) throw Error('Usage: --expected <result.json> --actual <result.json> [--output <comparison.json>]');
if (maxTicks !== null && (!Number.isSafeInteger(maxTicks) || maxTicks < 1)) throw Error('Invalid --max-ticks');
const workspace = resolve(dirname(fileURLToPath(import.meta.url)), '../../..'), commonRoot = resolve(workspace, 'eagler-common');
const {compareTraces} = await import(pathToFileURL(resolve(commonRoot, 'testkit/replay-verifier/compare.mjs')));
const common = await import(pathToFileURL(resolve(commonRoot, 'testkit/replay-verifier/adapter.mjs')));
const digest = path => createHash('sha256').update(readFileSync(path)).digest('hex');
async function* jsonLines(path) {
  const input = createReadStream(path, {encoding: 'utf8'}), lines = createInterface({input, crlfDelay: Infinity});
  try { for await (const line of lines) if (line.trim()) yield JSON.parse(line); }
  finally { lines.close(); input.destroy(); }
}
const expected = JSON.parse(readFileSync(expectedPath, 'utf8')), actual = JSON.parse(readFileSync(actualPath, 'utf8'));
if (option('--replay-sha')) expected.replay.sha256 = actual.replay.sha256 = option('--replay-sha');
if (expected.replay.sha256 !== actual.replay.sha256) throw Error('Replay identity mismatch');
if (option('--expected-rows')) expected.rows.path = resolve(option('--expected-rows'));
if (option('--actual-rows')) actual.rows.path = resolve(option('--actual-rows'));
const fixture = {replaySha256: expected.replay.sha256};
const original = resolve(oracleRoot, '../[th10] 东方风神录 (汉化版+日文版)');
const identity = {executableSha256: digest(resolve(original, 'th10.exe')), resourceSha256: digest(resolve(original, 'th10.dat'))};
const result = await compareTraces(
  recordsFromLongJsonLines({fixture, rows: jsonLines(expected.rows.path), provider: expected.provider, identity, complete: maxTicks === null && expected.complete, limit: maxTicks}, common),
  recordsFromLongJsonLines({fixture, rows: jsonLines(actual.rows.path), provider: actual.provider, identity, complete: maxTicks === null && actual.complete, limit: maxTicks}, common),
);
if (output) writeFileSync(output, `${JSON.stringify(result, null, 2)}\n`, 'utf8');
console.log(JSON.stringify(result, null, 2)); process.exitCode = result.status === 'PASS' ? 0 : 1;
