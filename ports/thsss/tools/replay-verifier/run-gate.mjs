#!/usr/bin/env node
import {spawnSync} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {framesOf, loadRaw} from './observation.mjs';
import {attribute} from './attribute.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const repo = resolve(here, '../..');
const common = resolve(process.env.EAGLER_COMMON_ROOT || resolve(repo, '../../common'));
const args = options(process.argv.slice(2));
if (args.negative === '1') {
  const run = spawnSync(process.execPath, ['--test', resolve(here, 'attribute.test.mjs')], {stdio: 'inherit'});
  process.exit(run.status ?? 1);
}
const {compareFileSuite, acceptFileSuite} = await import(pathToFileURL(resolve(common, 'testkit/replay-verifier/file-suite.mjs')));
const corpus = JSON.parse(readFileSync(resolve(here, 'corpus.json'), 'utf8'));
const captureRoot = resolve(args.output || resolve(repo, 'artifacts/replay-verifier'));
const goldenRoot = resolve(here, 'golden');
if (args.accept === '1') {
  const manifest = await acceptFileSuite({corpus, captureRoot, goldenRoot});
  console.log(JSON.stringify({accepted: Object.keys(manifest.assets)}, null, 2));
  process.exit(0);
}
const result = await compareFileSuite({
  corpus,
  captureRoot,
  goldenRoot: args.golden === '1' ? goldenRoot : undefined,
  lane: args.suite || 'quick',
  report: resolve(captureRoot, 'report.json'),
});
for (const item of result.results) {
  const id = item.id;
  let expected = [];
  let actual = [];
  try { expected = framesOf(loadRaw(resolve(captureRoot, `${id}.original.raw.jsonl`))); } catch { /* golden lane has no raw */ }
  try { actual = framesOf(loadRaw(resolve(captureRoot, `${id}.candidate.raw.jsonl`))); } catch { /* candidate raw is optional */ }
  const judged = attribute(item, expected, actual);
  item.attributionHint = judged.attributionHint;
  item.fieldDiff = judged.fieldDiff;
  console.log(`${id}: ${item.status} ${item.attributionHint || ''} ${item.reason || ''} ticks=${item.comparedTicks ?? 0}`);
  if (item.fieldDiff) console.log('  field ' + JSON.stringify(item.fieldDiff));
}
console.log(result.status);
process.exit(result.status === 'PASS' ? 0 : 1);

function options(argv) {
  const out = {};
  for (let i = 0; i < argv.length; i += 2) {
    if (!argv[i]?.startsWith('--')) continue;
    out[argv[i].slice(2)] = argv[i + 1];
  }
  return out;
}
