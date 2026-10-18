import {mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {dirname, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const sourcePath = fileURLToPath(new URL('../browser/spectator-timeline-view.mjs', import.meta.url));
const source = readFileSync(sourcePath);
const args = process.argv.slice(2);
const check = args.includes('--check');
const targets = [];
for (let index = 0; index < args.length; ++index) {
  if (args[index] === '--check') continue;
  if (args[index] === '--target') {
    const target = args[index + 1];
    if (!target || target.startsWith('--')) throw new Error('--target requires a path');
    targets.push(target);
    ++index;
  } else {
    targets.push(args[index]);
  }
}
if (targets.length === 0) throw new Error('Usage: node tools/sync-spectator-timeline-view.mjs [--check] [--target] PATH ...');

for (const value of targets) {
  const target = resolve(value);
  if (target === resolve(sourcePath)) throw new Error('Refusing to target the shared source itself');
  let current;
  try { current = readFileSync(target); }
  catch (error) {
    if (check) throw new Error(`Missing generated spectator timeline target: ${target}`, {cause: error});
    current = null;
  }
  if (current && current.equals(source)) {
    console.log(`${check ? 'CHECK' : 'UNCHANGED'} ${target}`);
    continue;
  }
  if (check) throw new Error(`Stale spectator timeline target: ${target}`);
  mkdirSync(dirname(target), {recursive: true});
  writeFileSync(target, source);
  console.log(`SYNC ${target}`);
}
