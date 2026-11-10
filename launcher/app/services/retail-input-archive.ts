import sevenZipWasmUrl from '7z-wasm/7zz.wasm?url';
import unrarWasmUrl from 'node-unrar-js/esm/js/unrar.wasm?url';
import {EncryptedZipError, parseStoredZip, readOriginalZipEntry} from '../../package/stored-zip.mjs';
import {GAME_DATA_PACK_SCHEMA, OFFLINE_GAME_PACK_SCHEMA} from '../../compat/legacy-game-pack.mjs';
import {hasRetailDatHeader} from '../../src/launcher/retail-archive.mjs';
import type {RetailInput} from '../../src/launcher/retail-package.mjs';

import {archiveLimit, selectRetailArchiveEntries} from './retail-archive-entries';

export class RetailArchivePasswordError extends Error {
  override name = 'RetailArchivePasswordError';
}

export async function isOriginalGameSelection(inputs: readonly (Blob & {name?: string})[]) {
  if (inputs.length !== 1) return true;
  const input = inputs[0];
  if (!input.name || /\.data$/i.test(input.name)) return false;
  const kind = await archiveType(input as RetailInput);
  if (kind === 'rar' || kind === '7z') return true;
  if (kind !== 'zip') return hasRetailDatHeader(input);
  const entries = await parseOriginalZip(input);
  if (!entries) return true;
  if (entries.has('package.json')) return false;
  const manifest = entries.get('manifest.json');
  if (manifest && manifest.uncompressedSize <= 1024 * 1024) {
    const content = await readOriginalZipEntry(input, manifest);
    try {
      const {schema} = JSON.parse(await content.text());
      if (schema === GAME_DATA_PACK_SCHEMA || schema === OFFLINE_GAME_PACK_SCHEMA) return false;
    } catch { /* An original game archive may contain unrelated JSON. */ }
  }
  return true;
}

export function retailInputAtPath(blob: Blob, path: string): RetailInput {
  return Object.assign(blob, {name: path.split('/').at(-1)!, archivePath: path});
}

async function archiveType(input: RetailInput) {
  const head = new Uint8Array(await input.slice(0, 8).arrayBuffer());
  const starts = (bytes: number[]) => bytes.every((value, index) => head[index] === value);
  if (starts([0x50, 0x4b, 3, 4]) || starts([0x50, 0x4b, 5, 6])) return 'zip';
  if (starts([0x52, 0x61, 0x72, 0x21, 0x1a, 7, 0]) || starts([0x52, 0x61, 0x72, 0x21, 0x1a, 7, 1, 0])) return 'rar';
  if (starts([0x37, 0x7a, 0xbc, 0xaf, 0x27, 0x1c])) return '7z';
  if (/\.(?:zip|rar|7z)$/i.test(input.name)) throw Error('Unrecognized archive header');
  return null;
}

async function parseOriginalZip(input: Blob) {
  try {return await parseStoredZip(input, {originalGame: true});}
  catch (error) {
    if (error instanceof EncryptedZipError) return null;
    throw error;
  }
}

async function extractInWorker(kind: 'rar' | 'zip' | '7z', input: RetailInput, limits: Record<string, number>,
  password: string, signal?: AbortSignal): Promise<RetailInput[]> {
  signal?.throwIfAborted();
  const worker = kind === 'rar'
    ? new Worker(new URL('./retail-rar.worker.ts', import.meta.url), {type: 'module'})
    : new Worker(new URL('./retail-7zip.worker.ts', import.meta.url), {type: 'module'});
  const wasmUrl = new URL(kind === 'rar' ? unrarWasmUrl : sevenZipWasmUrl, location.href).href;
  try {
    return await new Promise((resolve, reject) => {
      function cleanup() {signal?.removeEventListener('abort', abort);}
      function abort() {cleanup(); reject(signal?.reason ?? new DOMException('Aborted', 'AbortError'));}
      signal?.addEventListener('abort', abort, {once: true});
      worker.onmessage = ({data}: MessageEvent<{files?: {path: string; blob: Blob}[]; error?: string; passwordRequired?: boolean}>) => {
        cleanup();
        if (data.files) resolve(data.files.map(({path, blob}) => retailInputAtPath(blob, path)));
        else if (data.passwordRequired) reject(new RetailArchivePasswordError(data.error));
        else reject(Error(data.error || 'Unable to read the game archive'));
      };
      worker.onerror = () => {cleanup(); reject(Error('Local archive extractor stopped'));};
      worker.postMessage({input, kind, limits, password, wasmUrl});
    });
  } finally {worker.terminate();}
}

export async function expandRetailInputs(inputs: readonly RetailInput[], limits: Record<string, number>, password: string, signal?: AbortSignal) {
  const files: RetailInput[] = [];
  const archiveLimits = {...limits};
  for (const input of inputs) {
    const name = input.name.toLowerCase();
    if (!input.webkitRelativePath && name === 'thbgm.dat') delete archiveLimits[name];
  }
  for (const input of inputs) {
    signal?.throwIfAborted();
    const kind = input.webkitRelativePath ? null : await archiveType(input);
    if (!kind) {files.push(input); continue;}
    if (input.size > archiveLimit) throw Error('Game archive is too large');
    const entries = kind === 'zip' ? await parseOriginalZip(input) : null;
    if (!entries) {
      files.push(...await extractInWorker(kind, input, archiveLimits, password, signal));
      continue;
    }
    for (const {entry, path} of selectRetailArchiveEntries(entries.values(), archiveLimits, entry => ({path: entry.name, size: entry.uncompressedSize}))) {
      signal?.throwIfAborted();
      files.push(retailInputAtPath(await readOriginalZipEntry(input, entry, signal), path));
    }
  }
  return files;
}
