import {createRetailPackage, type RetailInput, type RetailPackage, type RetailTrack} from '../../src/launcher/retail-package.mjs';
import {buildStoredZip} from '../../package/stored-zip.mjs';
import wasmUrl from '@ffmpeg/core/wasm?url';
import type {GameId} from '../../src/contracts/product-catalog.mts';
import type {SiteGameData} from '../../src/contracts/site-manifest.mts';

export type {RetailInput} from '../../src/launcher/retail-package.mjs';

export async function convertRetailPackage(options: {
  game: GameId;
  expected?: SiteGameData;
  inputs: readonly RetailInput[];
  fontTargets?: readonly string[];
  loadFont?(target: string): Promise<Blob | null>;
  onTrack?(track: RetailTrack): void;
  signal?: AbortSignal;
}): Promise<RetailPackage> {
  let worker: Worker | undefined;
  function encodeOgg(input: Blob, {pcm, signal}: {pcm: boolean; signal?: AbortSignal}): Promise<Blob> {
    signal?.throwIfAborted();
    worker ??= new Worker(new URL('./retail-ogg.worker.ts', import.meta.url), {type: 'module'});
    const encoder = worker;
    return new Promise((resolve, reject) => {
      function cleanup() {
        signal?.removeEventListener('abort', abort);
        encoder.onmessage = null;
        encoder.onerror = null;
      }
      function abort() {cleanup(); reject(signal?.reason ?? new DOMException('Aborted', 'AbortError'));}
      signal?.addEventListener('abort', abort, {once: true});
      encoder.onerror = () => {cleanup(); reject(Error('Local audio encoder stopped'));};
      encoder.onmessage = ({data}) => {
        cleanup();
        if (data.blob instanceof Blob) resolve(data.blob);
        else reject(Error(data.error || 'Local audio conversion failed'));
      };
      encoder.postMessage({input, pcm, wasmUrl: new URL(wasmUrl, location.href).href});
    });
  }
  try {return await createRetailPackage({...options, encodeOgg});}
  finally {worker?.terminate();}
}

export async function downloadRetailPackage({descriptor, payloads}: RetailPackage, signal?: AbortSignal): Promise<Blob> {
  const manifest = new Blob([JSON.stringify(descriptor)], {type: 'application/json'});
  const entries: [string, Blob][] = [['package.json', manifest]];
  for (const [id, blob] of payloads) entries.push([descriptor.files[id].source, blob]);
  return buildStoredZip(entries, signal);
}
