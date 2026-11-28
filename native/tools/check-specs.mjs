import { readFile, readdir, stat } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { createHash } from 'node:crypto';

const docs = fileURLToPath(new URL('../docs/', import.meta.url));
const summaries = new Set(['baseline.md', 'acceptance.md', 'comparison.md']);
const definition = /^[A-Z][A-Z0-9]*(?:-[A-Z]+)*-\d+(?:\.\d+[a-z]?)?$/;
const definitions = new Map();
const failures = [];
let links = 0;

for (const name of (await readdir(docs)).filter(name => name.endsWith('.md')).sort()) {
  const source = await readFile(path.join(docs, name), 'utf8');
  for (const [index, line] of source.split('\n').entries()) {
    const id = line.match(/^\|\s*([^|]+?)\s*\|/)?.[1];
    if (!summaries.has(name) && id && definition.test(id)) {
      const here = `${name}:${index + 1}`;
      const previous = definitions.get(id);
      if (previous) failures.push(`${id}: ${previous} and ${here}`);
      else definitions.set(id, here);
    }
  }
  for (const match of source.matchAll(/\]\(([^)\s]+)\)/g)) {
    const target = match[1].split('#')[0];
    if (!target || /^[a-z]+:/i.test(target)) continue;
    try {
      await stat(path.resolve(docs, target));
      links++;
    } catch {
      failures.push(`${name}: missing ${target}`);
    }
  }
}

if (process.argv.includes('--current')) {
  const root = path.dirname(docs);
  const manifest = JSON.parse(await readFile(path.join(docs, 'evidence/current/manifest.json'), 'utf8'));
  const handoff = JSON.parse(await readFile(path.join(docs, 'evidence/windows/ui-handoff.json'), 'utf8'));
  const build = manifest.productionSourceMatchesWindowsPackage;
  if (!build?.sha256 || !build?.aotSha256 || !Object.keys(build?.source ?? {}).length) {
    failures.push('Current screenshots have no Windows package source receipt');
  }
  if (handoff.packageSha256 !== build?.sha256 || handoff.aotSha256 !== build?.aotSha256) {
    failures.push('Windows handoff does not identify the package recorded with current screenshots');
  }
  const mismatches = Object.entries(build?.source ?? {}).filter(([file, expected]) =>
    handoff.source?.[file] !== expected,
  ).map(([file]) => file);
  if (mismatches.length) {
    failures.push(`Windows handoff source differs from its package: ${mismatches.join(', ')}`);
  }
  for (const [file, expected] of Object.entries(build?.source ?? {})) {
    if (manifest.source[file] && manifest.source[file] !== expected) {
      failures.push(`Screenshot and Windows package source disagree: ${file}`);
    }
  }
  for (const [file, expected] of new Map([
    ...Object.entries(manifest.source),
    ...Object.entries(build?.source ?? {}),
    ...manifest.captures.map(item => [item.file, item.sha256]),
  ])) {
    const actual = createHash('sha256').update(await readFile(path.join(root, file))).digest('hex');
    if (actual !== expected) failures.push(`Current screenshots are stale or changed: ${file}`);
  }
  if (!failures.length) console.log(`${manifest.captures.length} screenshots and Windows handoff match their recorded source, image and package hashes`);
}

if (failures.length) {
  console.error(failures.join('\n'));
  process.exitCode = 1;
} else {
  console.log(`${definitions.size} unique requirement definitions | ${links} local links`);
}
