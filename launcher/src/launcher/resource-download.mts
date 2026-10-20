type Progress = (loaded: number, total: number, url: string) => void;
type Fetch = typeof globalThis.fetch;

export const resourcePath = (url: URL) => /^\/(?:games|shared|language|language-updates)\//.test(url.pathname);

/** Resource identity stays with the Package installer. Routes provide only
 * byte-identical publication locations; each attempt owns its native reader. */
export function createResourceDownloader({fetchImpl = globalThis.fetch, idleMs = 15_000,
  routeTimeoutMs = 5_000, slowWindowMs = 10_000, minimumRate = 64 * 1024}:
  {fetchImpl?: Fetch; idleMs?: number; routeTimeoutMs?: number; slowWindowMs?: number; minimumRate?: number} = {}) {
  const unsupported = new Set<string>();
  const preferred = new Map<string, string>();

  async function routes(url: URL, signal?: AbortSignal | null): Promise<{sources: string[]; bytes?: number}> {
    const scope = url.origin;
    if (unsupported.has(scope)) return {sources: []};
    const query = new URL('/_eagler/resource-routes', url); query.searchParams.set('path', url.pathname + url.search);
    const controller = new AbortController(), cancel = () => controller.abort(signal?.reason);
    const timer = setTimeout(() => controller.abort(), routeTimeoutMs);
    signal?.addEventListener('abort', cancel, {once: true});
    if (signal?.aborted) cancel();
    try {
      const response = await fetchImpl(query, {cache: 'no-store', redirect: 'error', signal: controller.signal});
      if (!response.ok || !/^application\/json\b/i.test(response.headers.get('content-type') || '')) {
        await response.body?.cancel();
        if (response.status === 404 || response.status === 405 || response.ok) unsupported.add(scope);
        return {sources: []};
      }
      const text = await response.text();
      if (text.length > 65_536) return {sources: []};
      const value = JSON.parse(text);
      if (value.schema !== 'eagler-touhou/resource-routes/1' || value.path !== url.pathname ||
          !Number.isSafeInteger(value.bytes) || value.bytes < 0 || !/^[a-f0-9]{64}$/.test(value.sha256) ||
          !Array.isArray(value.sources) || value.sources.length > 8) return {sources: []};
      const sources: string[] = [];
      for (const source of value.sources) {
        const next = new URL(source);
        if ((next.protocol !== 'https:' && next.origin !== url.origin) || next.username || next.password ||
            next.hash || next.pathname !== url.pathname || next.searchParams.toString() !== url.searchParams.toString()) return {sources: []};
        if (!sources.includes(next.href)) sources.push(next.href);
      }
      const good = preferred.get(url.origin);
      return {bytes: value.bytes, sources: good ? [...sources.filter(source => new URL(source).origin === good),
        ...sources.filter(source => new URL(source).origin !== good)] : sources};
    } catch {return {sources: []};}
    finally {clearTimeout(timer); signal?.removeEventListener('abort', cancel);}
  }

  async function attempt(url: string, init: RequestInit, slowCheck: boolean, progress: Progress, expectedBytes?: number) {
    const controller = new AbortController(), signal = init.signal;
    let idle: ReturnType<typeof setTimeout>, slow: ReturnType<typeof setTimeout> | undefined;
    let loaded = 0, failure: Error | null = null, reader: ReadableStreamDefaultReader<Uint8Array> | undefined;
    const cancel = () => controller.abort(signal?.reason);
    const timeout = (message: string) => {failure = new DOMException(message, 'TimeoutError'); controller.abort(failure);};
    const arm = () => {clearTimeout(idle); idle = setTimeout(() => timeout('Resource download stopped receiving bytes'), idleMs);};
    signal?.addEventListener('abort', cancel, {once: true});
    if (signal?.aborted) cancel();
    arm();
    if (slowCheck) slow = setTimeout(() => {
      if (loaded < minimumRate * slowWindowMs / 1000) timeout('Resource download is slow; trying another node');
    }, slowWindowMs);
    try {
      const response = await fetchImpl(url, {...init, signal: controller.signal});
      if (!response.ok) {await response.body?.cancel(); throw new Error(`HTTP ${response.status}`);}
      const total = expectedBytes ?? (Number(response.headers.get('content-length')) || 0);
      progress(0, total, url);
      const chunks: BlobPart[] = [];
      if (response.body) {
        reader = response.body.getReader();
        while (true) {
          const part = await reader.read(); if (part.done) break;
          if (part.value.byteLength) {
            chunks.push(part.value as Uint8Array<ArrayBuffer>); loaded += part.value.byteLength; arm(); progress(loaded, total, url);
          }
        }
      } else {const data = await response.blob(); chunks.push(data); loaded = data.size;}
      if (signal?.aborted) throw signal.reason ?? new DOMException('Aborted', 'AbortError');
      if (expectedBytes !== undefined && loaded !== expectedBytes) throw new Error(`Resource size mismatch (${loaded}/${expectedBytes})`);
      const blob = new Blob(chunks, {type: response.headers.get('content-type') || 'application/octet-stream'});
      const result = new Response(blob, {status: response.status, statusText: response.statusText, headers: response.headers});
      Object.defineProperty(result, 'url', {value: response.url || url});
      progress(loaded, total || loaded, url); return result;
    } catch (error) {throw failure ?? error;}
    finally {
      clearTimeout(idle!); clearTimeout(slow); signal?.removeEventListener('abort', cancel);
      if (reader) {await reader.cancel().catch(() => {}); reader.releaseLock();}
    }
  }

  return async (url: URL, init: RequestInit = {}, progress: Progress = () => {}) => {
    if (init.signal?.aborted) throw init.signal.reason ?? new DOMException('Aborted', 'AbortError');
    const route = await routes(url, init.signal), locations = route.sources;
    if (init.signal?.aborted) throw init.signal.reason ?? new DOMException('Aborted', 'AbortError');
    const sources = locations.length ? locations : [url.href];
    let error: unknown;
    for (const [index, source] of sources.entries()) {
      if (init.signal?.aborted) throw init.signal.reason ?? new DOMException('Aborted', 'AbortError');
      const headers = new Headers(init.headers);
      if (new URL(source).origin !== url.origin) headers.delete('Authorization');
      try {
        const response = await attempt(source, {...init, headers, credentials: new URL(source).origin === url.origin ? init.credentials : 'omit'},
          index < sources.length - 1, progress, route.bytes);
        if (locations.length) preferred.set(url.origin, new URL(source).origin);
        return response;
      } catch (cause) {
        if (init.signal?.aborted) throw init.signal.reason ?? new DOMException('Aborted', 'AbortError');
        error = cause; if (preferred.get(url.origin) === new URL(source).origin) preferred.delete(url.origin);
      }
    }
    throw error;
  };
}
