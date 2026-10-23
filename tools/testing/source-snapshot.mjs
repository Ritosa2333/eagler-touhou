import {execFileSync} from 'node:child_process';
import {readFileSync, lstatSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';

/** Content identity survives staging and rebasing and includes local test edits. */
export function sourceSnapshot(root) {
  const git = args => execFileSync('git', args, {cwd:root, encoding:'utf8', windowsHide:true, maxBuffer:32*1024*1024});
  const revision = git(['rev-parse', 'HEAD']).trim();
  const tracked = git(['ls-files', '--stage', '-z']).split('\0').filter(Boolean);
  const pins = tracked.filter(row=>row.startsWith('160000 ')).map(row=>row.split('\t'));
  const files = [...new Set(git(['ls-files', '--cached', '--others', '--exclude-standard', '-z'])
    .split('\0').filter(Boolean))].sort();
  const digest = createHash('sha256');
  let count = 0;
  for (const file of files) {
    const pin = pins.find(([, path])=>path === file);
    if (pin) {
      digest.update(file).update('\0').update(pin[0]).update('\0');
      const checkout = resolve(root, file);
      if (lstatSync(checkout, {throwIfNoEntry:false})?.isDirectory() &&
          lstatSync(resolve(checkout, '.git'), {throwIfNoEntry:false})) {
        const nested = sourceSnapshot(checkout);
        digest.update(nested.revision).update('\0').update(nested.sha256).update('\0');
        count += nested.files;
      }
      continue;
    }
    const path = resolve(root, file);
    let stat;
    try {stat = lstatSync(path);} catch (error) {
      if (error.code !== 'ENOENT') throw error;
      digest.update(file).update('\0missing\0'); continue;
    }
    if (!stat.isFile()) throw new Error(`Test input must be a regular file: ${file}`);
    let bytes = readFileSync(path);
    if (/\.(?:[cm]?js|mts|tsx?|jsx|json|cpp|cc|c|h|hpp|inc|html|css|md|py|ps1|yml|yaml|cmake|txt)$/.test(file))
      bytes = Buffer.from(bytes.toString('latin1').replaceAll('\r\n', '\n'), 'latin1');
    digest.update(file).update('\0').update(createHash('sha256').update(bytes).digest()).update('\0');
    count++;
  }
  return {revision, sha256:digest.digest('hex'), files:count};
}
