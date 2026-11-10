import {createEncoder} from 'wasm-media-encoders/esnext';

import {encodeRetailVorbis} from './retail-vorbis';

let core: ReturnType<typeof createEncoder<'audio/ogg'>> | undefined;

self.onmessage = async ({data}: MessageEvent<{input: Blob; pcm: boolean; wasmUrl: string}>) => {
  try {
    core ??= createEncoder('audio/ogg', data.wasmUrl);
    self.postMessage({blob: await encodeRetailVorbis(data.input, data.pcm, await core)});
  } catch (error) {
    self.postMessage({error: error instanceof Error ? error.message : String(error)});
  }
};
