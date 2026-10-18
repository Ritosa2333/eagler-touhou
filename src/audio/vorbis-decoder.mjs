// Keep one small Wasm runtime per URL. Decoder handles and encoded Wasm copies
// belong to each open operation and are freed by close(), including failures.
const runtimes = new Map();
export const DEFAULT_VORBIS_MODULE_URL = new URL('../../site/runtime/vorbis.mjs', import.meta.url).href;

function runtimeFor(moduleURL) {
  const url = String(moduleURL);
  if (!runtimes.has(url)) {
    const pending = import(url).then(({default:createDecoder}) => createDecoder({
      locateFile:file => new URL(file, url).href,
    })).catch(error => {
      runtimes.delete(url);
      throw new Error('OGG 流式解码器加载失败：' + error.message, {cause:error});
    });
    runtimes.set(url, pending);
  }
  return runtimes.get(url);
}

export async function openVorbisDecoder(encoded, {moduleURL=DEFAULT_VORBIS_MODULE_URL}={}) {
  const module = await runtimeFor(moduleURL);
  const bytes = encoded instanceof Uint8Array ? encoded : new Uint8Array(encoded);
  if (!bytes.length || bytes.length > 0x7fffffff) throw Error('OGG 编码文件大小无效');
  let input = 0, handle = 0, closed = false;
  const failure = operation => Error(`OGG ${operation}失败（Vorbis ${module._th12_vorbis_error()}）`);
  const close = () => {
    if (closed) return;
    closed = true;
    if (handle) module._th12_vorbis_close(handle);
    if (input) module._free(input);
    handle = input = 0;
  };
  try {
    input = module._malloc(bytes.length);
    if (!input) throw Error('OGG 解码器内存不足');
    module.HEAPU8.set(bytes, input);
    handle = module._th12_vorbis_open(input, bytes.length);
    if (!handle) throw failure('打开');
    const channels = module._th12_vorbis_channels(handle);
    const sampleRate = module._th12_vorbis_rate(handle);
    const length = module._th12_vorbis_length(handle) >>> 0;
    const capacity = module._th12_vorbis_max_frames();
    const assertOpen = () => { if (closed) throw Error('OGG 解码器已经关闭'); };
    return {
      channels, sampleRate, length, capacity, duration:length / sampleRate,
      seek(frame) {
        assertOpen();
        if (!Number.isInteger(frame) || frame < 0 || frame >= length) throw Error('OGG 样本位置无效');
        if (!module._th12_vorbis_seek(handle, frame)) throw failure('定位');
      },
      // The returned views are valid only until the next read/seek or Wasm
      // allocation. Copy straight into the bounded Web Audio queue.
      read(frames) {
        assertOpen();
        if (!Number.isInteger(frames) || frames < 1 || frames > capacity) throw Error('OGG 解码块大小无效');
        const count = module._th12_vorbis_read(handle, frames);
        if (count < 0) throw failure('解码');
        const base = module._th12_vorbis_pcm(handle) >>> 2;
        return {frames:count, channels:Array.from({length:channels}, (_, channel) =>
          module.HEAPF32.subarray(base + channel * capacity, base + channel * capacity + count))};
      },
      close,
    };
  } catch (error) {
    close();
    throw error;
  }
}

export async function inspectVorbis(encoded, options) {
  const decoder = await openVorbisDecoder(encoded, options);
  try {
    const {channels, sampleRate, length, duration} = decoder;
    return {channels, sampleRate, length, duration};
  } finally { decoder.close(); }
}
