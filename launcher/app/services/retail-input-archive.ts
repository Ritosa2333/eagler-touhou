import unrarWasmUrl from 'node-unrar-js/esm/js/unrar.wasm?url';
import {parseStoredZip, readOriginalZipEntry} from '../../package/stored-zip.mjs';
import type {RetailInput} from '../../src/launcher/retail-package.mjs';

import {archiveLimit, selectRetailArchiveEntries} from './retail-archive-entries';

export function retailInputAtPath(blob: Blob, path: string): RetailInput {
  return Object.assign(blob, {name: path.split('/').at(-1)!, archivePath: path});
}

async function archiveType(input: RetailInput) {
  const head = new Uint8Array(await input.slice(0, 8).arrayBuffer());
  const starts = (bytes: number[]) => bytes.every((value, index) => head[index] === value);
  if (starts([0x50, 0x4b, 3, 4]) || starts([0x50, 0x4b, 5, 6])) return 'zip';
  if (starts([0x52, 0x61, 0x72, 0x21, 0x1a, 7, 0]) || starts([0x52, 0x61, 0x72, 0x21, 0x1a, 7, 1, 0])) return 'rar';
  if (/\.(?:zip|rar)$/i.test(input.name)) throw Error('Unrecognized ZIP or RAR header');
  return null;
}

async function extractRar(input: RetailInput, limits: Record<string, number>, password: string, signal?: AbortSignal): Promise<RetailInput[]> {
  signal?.throwIfAborted();
  const worker = new Worker(new URL('./retail-rar.worker.ts', import.meta.url), {type: 'module'});
  try {
    return await new Promise((resolve, reject) => {
      function cleanup() {signal?.removeEventListener('abort', abort);}
      function abort() {cleanup(); reject(signal?.reason ?? new DOMException('Aborted', 'AbortError'));}
      signal?.addEventListener('abort', abort, {once: true});
      worker.onmessage = ({data}: MessageEvent<{files?: {path: string; blob: Blob}[]; error?: string}>) => {
        cleanup();
        if (data.files) resolve(data.files.map(({path, blob}) => retailInputAtPath(blob, path)));
        else reject(Error(data.error || 'Unable to read the RAR archive'));
      };
      worker.onerror = () => {cleanup(); reject(Error('Local RAR extractor stopped'));};
      worker.postMessage({input, limits, password, wasmUrl: new URL(unrarWasmUrl, location.href).href});
    });
  } finally {worker.terminate();}
}

export async function expandRetailInputs(inputs: readonly RetailInput[], limits: Record<string, number>, password: string, signal?: AbortSignal) {
  const files: RetailInput[] = [];
  const archiveLimits = {...limits};
  for (const input of inputs) {
    const name = input.name.toLowerCase();
    if (!input.webkitRelativePath && ['thbgm.dat', 'msgothic.ttc', 'unifont.otf'].includes(name)) delete archiveLimits[name];
  }
  for (const input of inputs) {
    signal?.throwIfAborted();
    const kind = input.webkitRelativePath ? null : await archiveType(input);
    if (!kind) {files.push(input); continue;}
    if (input.size > archiveLimit) throw Error('Game archive is too large');
    if (kind === 'rar') {
      files.push(...await extractRar(input, archiveLimits, password, signal));
      continue;
    }
    const entries = await parseStoredZip(input, {originalGame: true});
    for (const {entry, path} of selectRetailArchiveEntries(entries.values(), archiveLimits, entry => ({path: entry.name, size: entry.uncompressedSize}))) {
      signal?.throwIfAborted();
      files.push(retailInputAtPath(await readOriginalZipEntry(input, entry, signal), path));
    }
  }
  return files;
}
