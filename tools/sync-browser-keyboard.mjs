import {readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';

const begin = '      // BEGIN SHARED BROWSER KEYBOARD OWNERS';
const end = '      // END SHARED BROWSER KEYBOARD OWNERS';
const source = (await readFile(new URL('../browser/keyboard-owners.mjs', import.meta.url), 'utf8')).replaceAll('\r\n', '\n');
const inline = source.replace(/^export /gm, '').trim().split('\n').map(line => line ? `      ${line}` : '').join('\n');
const replacement = `${begin}\n      // Generated from eagler-common/browser/keyboard-owners.mjs.\n${inline}\n${end}`;
const args = process.argv.slice(2);
const check = args.includes('--check');
const shells = args.filter(arg => arg !== '--check');
if (!shells.length) throw new Error('usage: node tools/sync-browser-keyboard.mjs [--check] SHELL.html ...');
for (const shell of shells) {
  const path = resolve(shell);
  const html = await readFile(path, 'utf8');
  const block = html.includes('\r\n') ? replacement.replaceAll('\n', '\r\n') : replacement;
  const start = html.indexOf(begin), finish = html.indexOf(end, start);
  if (start < 0 || finish < 0 || html.indexOf(begin, start + begin.length) >= 0)
    throw new Error(`missing/duplicate shared keyboard markers: ${path}`);
  const result = html.slice(0, start) + block + html.slice(finish + end.length);
  if (check && html !== result) throw new Error(`shared keyboard source differs: ${path}`);
  if (!check && html !== result) await writeFile(path, result);
  console.log(`${check ? 'PASS' : 'SYNC'} shared browser keyboard: ${path}`);
}
