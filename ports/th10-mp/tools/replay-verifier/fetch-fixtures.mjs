#!/usr/bin/env node
import {createHash} from 'node:crypto';
import {existsSync} from 'node:fs';
import {mkdir, readFile, writeFile} from 'node:fs/promises';
import {dirname, join, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const root = resolve(here, '..', '..');
const corpus = JSON.parse(await readFile(join(here, 'corpus.json'), 'utf8'));
for (const fixture of corpus.cases.filter(entry => entry.sourceUrl && entry.replaySha256)) {
  const destination = resolve(root, fixture.source);
  if (existsSync(destination)) {
    const present = createHash('sha256').update(await readFile(destination)).digest('hex');
    if (present === fixture.replaySha256) {
      process.stdout.write(`${fixture.id}: present\n`);
      continue;
    }
    throw Error(`${fixture.id}: existing fixture has unexpected SHA-256; refusing to overwrite ${destination}`);
  }
  const response = await fetch(fixture.sourceUrl);
  if (!response.ok) throw Error(`${fixture.id}: download failed with HTTP ${response.status}`);
  const bytes = Buffer.from(await response.arrayBuffer());
  const actual = createHash('sha256').update(bytes).digest('hex');
  if (actual !== fixture.replaySha256) throw Error(`${fixture.id}: downloaded SHA-256 ${actual} does not match corpus`);
  await mkdir(dirname(destination), {recursive: true});
  await writeFile(destination, bytes, {flag: 'wx'});
  process.stdout.write(`${fixture.id}: downloaded and verified\n`);
}
