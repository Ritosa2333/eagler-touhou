import {createHash} from 'node:crypto';
import {readFileSync} from 'node:fs';
import {relative, resolve, sep} from 'node:path';
import {fileURLToPath} from 'node:url';

const root = resolve(fileURLToPath(new URL('../fixtures/ui-history/', import.meta.url)));
const manifest = JSON.parse(readFileSync(resolve(root, 'manifest.json'), 'utf8'));
if (manifest.schema !== 'eagler-touhou/ui-history-fixtures/1' || !Array.isArray(manifest.entries)) {
  throw new Error('Invalid UI history fixture manifest');
}

export const UI_HISTORY_FIXTURES = Object.freeze(manifest.entries.map(entry => Object.freeze({...entry})));
const entries = new Map(UI_HISTORY_FIXTURES.map(entry => [`${entry.revision}/${entry.path}`, entry]));
if (entries.size !== UI_HISTORY_FIXTURES.length) throw new Error('Duplicate UI history fixture entry');

function bytesFor(revision, path) {
  if (!/^[a-f0-9]{40}$/.test(revision)) throw new Error(`Invalid UI history revision: ${revision}`);
  const normalized = String(path).replaceAll('\\', '/');
  if (!normalized || normalized.startsWith('/') || normalized.split('/').some(part => !part || part === '.' || part === '..')) {
    throw new Error(`Invalid UI history path: ${path}`);
  }
  const entry = entries.get(`${revision}/${normalized}`);
  if (!entry) throw new Error(`Unregistered UI history fixture: ${revision}:${normalized}`);
  const file = resolve(root, revision, ...normalized.split('/'));
  const within = relative(root, file);
  if (!within || within === '..' || within.startsWith(`..${sep}`)) throw new Error(`UI history fixture escapes root: ${normalized}`);
  const bytes = readFileSync(file);
  const actual = createHash('sha256').update(bytes).digest('hex');
  if (bytes.length !== entry.bytes || actual !== entry.sha256) {
    throw new Error(`UI history fixture integrity failure: ${revision}:${normalized}`);
  }
  return bytes;
}

export function readUiHistoryBytes(revision, path) {
  return bytesFor(revision, path);
}

export function readUiHistoryText(revision, path) {
  return new TextDecoder('utf-8', {fatal: true}).decode(bytesFor(revision, path));
}

export function verifyUiHistoryFixtures() {
  for (const entry of UI_HISTORY_FIXTURES) bytesFor(entry.revision, entry.path);
  return UI_HISTORY_FIXTURES.length;
}
