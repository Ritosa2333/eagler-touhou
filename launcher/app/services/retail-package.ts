import {createRetailPackage, retailArchiveLimits, type RetailInput, type RetailPackage, type RetailTrack} from '../../src/launcher/retail-package.mjs';
import {expandRetailInputs} from './retail-input-archive';
import wasmUrl from 'wasm-media-encoders/wasm/ogg.wasm?url';
import type {GameId} from '../../src/contracts/product-catalog.mts';
import type {SiteGameData} from '../../src/contracts/site-manifest.mts';

export type {RetailInput} from '../../src/launcher/retail-package.mjs';

export async function convertRetailPackage(options: {
  game: GameId;
  expected?: SiteGameData;
  inputs: readonly RetailInput[];
  archivePassword?: string;
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
  try {
    const inputs = await expandRetailInputs(options.inputs, retailArchiveLimits(options.game), options.archivePassword ?? '', options.signal);
    return await createRetailPackage({...options, inputs, encodeOgg});
  }
  finally {worker?.terminate();}
}
