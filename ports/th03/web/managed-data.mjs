export const DATA_SCHEMA = 'eagler-touhou/pc98-content/1';
export const DATA_MAGIC = 'EPC98PK1';
export const DATA_LAYOUT_SPEC = Object.freeze({schema: DATA_SCHEMA, magic: DATA_MAGIC, header: 'magic8+manifestBytesLE32+utf8JSON', files: 'relative-path+offset+bytes+sha256'});
const text = new TextDecoder('utf-8', {fatal: true});
const digest = async blob => [...new Uint8Array(await crypto.subtle.digest('SHA-256', await blob.arrayBuffer()))].map(n => n.toString(16).padStart(2, '0')).join('');
export function contentPath(path) {
  if (typeof path !== 'string' || !path || /[\\:\0?#]/.test(path) || path.split('/').some(part => !part || part === '.' || part === '..')) throw Error('游戏资源路径无效');
  return path;
}
const mime = path => path.endsWith('.json') ? 'application/json' : /\.(ogg|opus)$/.test(path) ? 'audio/ogg' : path.endsWith('.wav') ? 'audio/wav' : path.endsWith('.svg') ? 'image/svg+xml' : 'application/octet-stream';
export class ManagedContent {
  static async open(value, game = 'th03') {
    const blob = new Blob([value]);
    if (blob.size < 14 || blob.size > 1024 * 1024 * 1024) throw Error('游戏资源包大小无效');
    const header = new Uint8Array(await blob.slice(0, 12).arrayBuffer());
    const size = new DataView(header.buffer).getUint32(8, true), base = 12 + size;
    if (text.decode(header.subarray(0, 8)) !== DATA_MAGIC || size < 2 || size > 4 * 1024 * 1024 || base > blob.size) throw Error('不是 PC-98 接入资源包');
    const manifest = JSON.parse(text.decode(await blob.slice(12, base).arrayBuffer()));
    if (manifest.schema !== DATA_SCHEMA || manifest.game !== game || !Array.isArray(manifest.files) || !manifest.files.length || manifest.files.length > 10000) throw Error('游戏资源包身份无效');
    const files = new Map(); let end = 0;
    for (const item of manifest.files) {
      const path = contentPath(item.path);
      if (files.has(path) || item.offset !== end || !Number.isSafeInteger(item.bytes) || item.bytes <= 0 || !/^[a-f0-9]{64}$/.test(item.sha256) || base + end + item.bytes > blob.size) throw Error('游戏资源包条目无效');
      files.set(path, item); end += item.bytes;
    }
    if (base + end !== blob.size) throw Error('游戏资源包长度无效');
    return new ManagedContent(blob.slice(base), [...files], game);
  }
  constructor(blob, entries, game) { this.blob = blob; this.files = new Map(entries); this.game = game; this.verified = new Map(); }
  async fetch(path) {
    const item = this.files.get(contentPath(path));
    if (!item) return new Response('Resource is not installed', {status: 404});
    const blob = this.blob.slice(item.offset, item.offset + item.bytes, mime(path));
    if (!this.verified.has(path)) {
      const check = digest(blob).then(hash => { if (hash !== item.sha256) throw Error('游戏资源校验失败：' + path); });
      this.verified.set(path, check);
    }
    await this.verified.get(path);
    return new Response(blob, {headers: {'Content-Type': mime(path), 'Content-Length': String(blob.size)}});
  }
  install(environment, base) {
    const nativeFetch = environment.fetch.bind(environment), root = new URL(base), content = this;
    environment.__eaglerPC98Content = this;
    environment.fetch = async (input, init) => {
      // The child iframe receives this method from its parent. URL/Request
      // instances cross realms, so instanceof URL would miss valid assets.
      const source = typeof input === 'string' ? input : typeof input?.url === 'string' ? input.url : input?.href;
      const url = new URL(source, environment.location.href);
      if (url.origin === root.origin && url.pathname.startsWith(root.pathname)) {
        const path = decodeURIComponent(url.pathname.slice(root.pathname.length));
        if (content.files.has(path)) {
          if ((init?.method ?? input.method ?? 'GET').toUpperCase() !== 'GET') throw Error('游戏资源为只读');
          const signal = init?.signal ?? input.signal; signal?.throwIfAborted();
          const response = await content.fetch(path); signal?.throwIfAborted(); return response;
        }
        // Never fall back to a server copy of missing game data.
        if (/\.(?:hdi|gz|exe|com|dat|bin|bmp|png|svg|woff2?|ogg|opus|wav|mid)$/i.test(path)) return new Response('Resource is not installed', {status: 404});
      }
      return nativeFetch(input, init);
    };
  }
}

export function installManagedWorkers(environment, content, base) {
  if (!environment.Worker) return;
  const NativeWorker = environment.Worker;
  const bootstrap = new URL('managed-worker.mjs', base).href;
  environment.Worker = class ManagedWorker extends NativeWorker {
    constructor(url, options = {}) {
      if (options.type !== 'module') throw Error('接入运行时需要模块 Worker');
      super(bootstrap, {...options, type: 'module'});
      super.postMessage({type: 'eagler-content-init', url: new URL(url, environment.location.href).href, base, game: content.game, blob: content.blob, entries: [...content.files]});
    }
  };
}
