type Progress = (loaded: number, total: number, url: string) => void;
type Fetch = typeof globalThis.fetch;
type Routes = {sources: string[]; bytes?: number};
type Policy = {unsupported: Set<string>; preferred: Map<string, string>; failed: Map<string, number>};
const policies = new WeakMap<Fetch, Policy>();
const aborted = (signal: AbortSignal) => signal.reason ?? new DOMException('Aborted', 'AbortError');
const timeout = (message: string) => new DOMException(message, 'TimeoutError');

// Bound even a fetch adapter or stream that does not settle when aborted.
function cancellable<T>(work: Promise<T>, signal: AbortSignal): Promise<T> {
  return new Promise((resolve, reject) => {
    const cancel = () => {signal.removeEventListener('abort', cancel); reject(aborted(signal));};
    signal.addEventListener('abort', cancel, {once: true});
    work.then(value => {signal.removeEventListener('abort', cancel); resolve(value);}, error => {
      signal.removeEventListener('abort', cancel); reject(error);
    });
    if (signal.aborted) cancel();
  });
}

export const resourcePath = (url: URL) => /^\/(?:games|shared|language|language-updates)\//.test(url.pathname);

/** Routes identify byte-identical locations. The Package installer still owns
 * complete-file integrity and publication. Policy is shared by a session's fetch. */
export function createResourceDownloader({fetchImpl = globalThis.fetch, idleMs = 4_000,
  routeTimeoutMs = 2_500, slowWindowMs = 3_000, minimumRate = 100 * 1024,
  probeBytes = 64 * 1024, probeWindowMs = 1_000, hedgeDelayMs = 400, cooldownMs = 60_000}:
  {fetchImpl?: Fetch; idleMs?: number; routeTimeoutMs?: number; slowWindowMs?: number; minimumRate?: number;
    probeBytes?: number; probeWindowMs?: number; hedgeDelayMs?: number; cooldownMs?: number} = {}) {
  let policy = policies.get(fetchImpl);
  if (!policy) {policy = {unsupported: new Set(), preferred: new Map(), failed: new Map()}; policies.set(fetchImpl, policy);}
  const {unsupported, preferred, failed} = policy;

  async function routes(url: URL, signal?: AbortSignal | null): Promise<Routes> {
    if (unsupported.has(url.origin)) return {sources: []};
    const query = new URL('/_eagler/resource-routes', url); query.searchParams.set('path', url.pathname + url.search);
    const controller = new AbortController(), cancel = () => controller.abort(signal?.reason);
    const timer = setTimeout(() => controller.abort(timeout('Resource route discovery timed out')), routeTimeoutMs);
    signal?.addEventListener('abort', cancel, {once: true});
    if (signal?.aborted) cancel();
    let response: Response | undefined;
    try {
      response = await cancellable(fetchImpl(query, {cache: 'no-store', redirect: 'error', signal: controller.signal}), controller.signal);
      if (!response.ok || !/^application\/json\b/i.test(response.headers.get('content-type') || '')) {
        if (response.status === 404 || response.status === 405 || response.ok) unsupported.add(url.origin);
        return {sources: []};
      }
      const text = await cancellable(response.text(), controller.signal);
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
      return {bytes: value.bytes, sources};
    } catch {return {sources: []};}
    finally {
      clearTimeout(timer); signal?.removeEventListener('abort', cancel); controller.abort();
      if (response?.body && !response.body.locked) void response.body.cancel().catch(() => {});
    }
  }

  return async (url: URL, init: RequestInit = {}, progress: Progress = () => {}) => {
    const signal = init.signal, check = () => {if (signal?.aborted) throw aborted(signal);};
    check();
    const route = await routes(url, signal); check();
    const explicitRange = new Headers(init.headers).has('Range');
    const resumable = route.bytes !== undefined && route.sources.length > 0 && !explicitRange;
    const sources = route.sources.length ? route.sources : [url.href];
    const exhausted = new Set<string>(), compared = new Set<string>();
    const origin = (source: string) => new URL(source).origin;
    const fail = (source: string, reason: unknown) => {
      if (!exhausted.has(source)) console.warn('[EAGLER|下载节点异常]', {node:origin(source), path:url.pathname,
        reason:reason instanceof Error ? reason.name : String(reason)});
      exhausted.add(source); failed.set(origin(source), Date.now() + cooldownMs);
      if (preferred.get(url.origin) === origin(source)) preferred.delete(url.origin);
    };
    const candidates = () => sources.filter(source => !exhausted.has(source)).sort((a, b) => {
      const cooling = (source: string) => (failed.get(origin(source)) ?? 0) > Date.now() ? 1 : 0;
      return cooling(a) - cooling(b) || Number(origin(b) === preferred.get(url.origin)) - Number(origin(a) === preferred.get(url.origin));
    });
    let chunks: BlobPart[] = [], loaded = 0, lastError: unknown;

    async function open(source: string, start: number, limit: number | undefined, controller: AbortController) {
      let idle: ReturnType<typeof setTimeout>, reader: ReadableStreamDefaultReader<Uint8Array> | undefined;
      let received = 0, ended = false;
      const cancel = () => controller.abort(signal?.reason);
      const arm = () => {clearTimeout(idle); idle = setTimeout(() => controller.abort(timeout('Resource download stopped receiving bytes')), idleMs);};
      const close = () => {
        clearTimeout(idle); signal?.removeEventListener('abort', cancel); controller.abort();
        if (reader) {void reader.cancel().catch(() => {}); reader.releaseLock(); reader = undefined;}
      };
      signal?.addEventListener('abort', cancel, {once: true});
      if (signal?.aborted) cancel();
      const headers = new Headers(init.headers), crossOrigin = origin(source) !== url.origin;
      if (crossOrigin) headers.delete('Authorization');
      if (!explicitRange && (start > 0 || limit !== undefined)) {
        headers.set('Range', 'bytes=' + start + '-' + (limit === undefined ? '' : Math.min(start + limit, route.bytes!) - 1));
      }
      arm();
      try {
        const request = fetchImpl(source, {...init, headers, credentials: crossOrigin ? 'omit' : init.credentials, signal: controller.signal});
        void request.then(response => {
          if (controller.signal.aborted && response.body && !response.body.locked) void response.body.cancel().catch(() => {});
        }, () => {});
        const response = await cancellable(request, controller.signal);
        if (!response.ok || ![200, 206].includes(response.status)) {
          void response.body?.cancel().catch(() => {}); throw new Error('HTTP ' + response.status);
        }
        let expected = explicitRange ? undefined : route.bytes;
        if (response.status === 206) {
          const match = /^bytes (\d+)-(\d+)\/(\d+)$/.exec(response.headers.get('Content-Range') || '');
          if (!match) throw new Error('Invalid resource Content-Range');
          const [, from, to, total] = match.map(Number);
          if (![from, to, total].every(Number.isSafeInteger) || from > to || to >= total || (route.bytes !== undefined && total !== route.bytes) ||
              (!explicitRange && (from !== start || to !== Math.min(start + (limit ?? total), total) - 1))) {
            throw new Error('Resource range does not match the requested bytes');
          }
          expected = to - from + 1;
        } else if (expected === undefined) {
          const length = response.headers.get('Content-Length');
          if (length !== null && /^\d+$/.test(length)) expected = Number(length);
        }
        reader = response.body?.getReader();
        const read = async (): Promise<Uint8Array | undefined> => {
          if (ended) return;
          const part = reader ? await cancellable(reader.read(), controller.signal) : {done: true, value: undefined};
          if (part.done) {
            ended = true; clearTimeout(idle);
            if (expected !== undefined && received !== expected) throw new Error('Resource size mismatch (' + received + '/' + expected + ')');
            return;
          }
          if (part.value?.byteLength) {
            received += part.value.byteLength; arm();
            if (expected !== undefined && received > expected) throw new Error('Resource exceeds declared size');
          }
          return part.value;
        };
        return {source, response, read, close, seed: [] as Uint8Array<ArrayBuffer>[],
          get received() {return received;}, get ended() {return ended;}};
      } catch (error) {close(); throw error;}
    }
    type Transfer = Awaited<ReturnType<typeof open>>;

    // At most two requests. Cold nodes deliver a bounded prefix; a known-good
    // node starts its full response and gets a delayed hedge.
    async function select(forced?: string): Promise<Transfer> {
      const queue = candidates();
      if (!queue.length) throw lastError ?? new Error('No resource nodes available');
      if (forced) {const i = queue.indexOf(forced); if (i >= 0) queue.unshift(...queue.splice(i, 1));}
      const warm = forced !== undefined || origin(queue[0]) === preferred.get(url.origin) || !resumable || queue.length === 1;
      return new Promise((resolve, reject) => {
        let settled = false, running = 0, timer: ReturnType<typeof setTimeout> | undefined;
        const controllers = new Set<AbortController>();
        const cancel = () => finish(undefined, aborted(signal!));
        const finish = (winner?: Transfer, error?: unknown) => {
          if (settled) return; settled = true; clearTimeout(timer); signal?.removeEventListener('abort', cancel);
          for (const controller of controllers) controller.abort();
          if (winner) resolve(winner); else reject(error);
        };
        const launch = () => {
          if (settled || running >= 2) return;
          if (running && queue[0] && (failed.get(origin(queue[0])) ?? 0) > Date.now()) return;
          const source = queue.shift();
          if (!source) {if (!running) finish(undefined, lastError ?? new Error('No resource nodes available')); return;}
          const controller = new AbortController(); controllers.add(controller); running++;
          const limit = resumable && (!warm || running > 1) && route.bytes! > 0 ? probeBytes : undefined;
          void (async () => {
            let transfer: Transfer | undefined;
            try {
              const began = performance.now();
              transfer = await open(source, resumable ? loaded : 0, limit, controller);
              do {
                const chunk = await transfer.read();
                if (chunk?.byteLength) transfer.seed.push(chunk as Uint8Array<ArrayBuffer>);
              } while (!transfer.ended && (transfer.received === 0 || (limit !== undefined &&
                transfer.received < Math.min(limit, route.bytes! - loaded) && performance.now() - began < probeWindowMs)));
              if (settled) {transfer.close(); return;}
              controllers.delete(controller); finish(transfer);
            } catch (error) {
              transfer?.close();
              if (!settled) {lastError = error; fail(source, error); running--; controllers.delete(controller); launch();}
            }
          })();
        };
        signal?.addEventListener('abort', cancel, {once: true});
        if (signal?.aborted) {cancel(); return;}
        launch();
        if (!settled) {if (warm) timer = setTimeout(launch, hedgeDelayMs); else launch();}
      });
    }

    let forced: string | undefined, nextTransfer: Transfer | undefined;
    let lastNode: string | undefined;
    while (candidates().length) {
      if (signal?.aborted) {nextTransfer?.close(); throw aborted(signal);}
      let active: Transfer | undefined, comparison: AbortController | undefined;
      let slow: ReturnType<typeof setInterval> | undefined, replacement: string | undefined;
      try {
        active = nextTransfer ?? await select(forced); nextTransfer = undefined; forced = undefined;
        const node = origin(active.response.url || active.source);
        if (node !== lastNode) {
          console.info('[EAGLER|下载]', {node, path:url.pathname, offset:loaded});
          lastNode = node;
        }
        // A server may ignore Range and send 200. Replace the prefix in that case.
        if (!resumable || active.response.status === 200) {chunks = []; loaded = 0;}
        const total = explicitRange ? Number(active.response.headers.get('Content-Length')) || 0 : (route.bytes ?? Number(active.response.headers.get('Content-Length'))) || 0;
        const append = (chunk: Uint8Array<ArrayBuffer>) => {chunks.push(chunk); loaded += chunk.byteLength; progress(loaded, total, active!.source);};
        progress(loaded, total, active.source);
        for (const chunk of active.seed) append(chunk);
        active.seed = [];
        let sampledAt = performance.now(), sampledBytes = loaded;
        if (resumable && slowWindowMs > 0) slow = setInterval(() => {
          const now = performance.now(), rate = (loaded - sampledBytes) * 1000 / (now - sampledAt);
          sampledAt = now; sampledBytes = loaded;
          if (comparison || rate >= minimumRate || route.bytes! - loaded <= probeBytes) return;
          const source = candidates().find(source => source !== active!.source && !compared.has(source) && (failed.get(origin(source)) ?? 0) <= Date.now());
          if (!source) return;
          compared.add(source);
          const controller = new AbortController(); comparison = controller;
          const began = performance.now(), offset = loaded;
          void (async () => {
            let probe: Transfer | undefined;
            let measured = false;
            const sampleEnd = timeout('Resource sampling window elapsed');
            const deadline = setTimeout(() => controller.abort(sampleEnd), probeWindowMs);
            try {
              probe = await open(source, offset, probeBytes, controller);
              while (!probe.ended && probe.received < probeBytes) {
                const chunk = await probe.read();
                if (chunk?.byteLength) probe.seed.push(chunk as Uint8Array<ArrayBuffer>);
              }
              measured = true;
            } catch (error) {
              if (controller.signal.reason === sampleEnd) {measured = !!probe?.received; if (!measured) fail(source, 'sampling-timeout');}
              else if (!signal?.aborted && (controller.signal.reason?.name !== 'AbortError' || error !== controller.signal.reason)) fail(source, error);
            } finally {clearTimeout(deadline);}
            try {
              const elapsed = Math.max(performance.now() - began, 1);
              const probeRate = (probe?.received ?? 0) * 1000 / elapsed, currentRate = (loaded - offset) * 1000 / elapsed;
              if (measured && !signal?.aborted && comparison === controller &&
                  (!controller.signal.aborted || controller.signal.reason === sampleEnd) && probeRate > Math.max(rate, currentRate) * 1.25) {
                replacement = source;
                // Reuse a completed sample, including a server's full 200 response.
                // Bytes received by the current stream during sampling overlap it.
                if (probe && !controller.signal.aborted && (probe.response.status === 200 || offset + probe.received >= loaded)) {
                  if (probe.response.status === 206) {
                    let overlap = loaded - offset;
                    probe.seed = probe.seed.flatMap(chunk => {
                      const skip = Math.min(overlap, chunk.byteLength); overlap -= skip;
                      return skip === chunk.byteLength ? [] : [chunk.subarray(skip)];
                    });
                  }
                  nextTransfer = probe; probe = undefined; comparison = undefined;
                }
                active!.close();
              }
            }
            finally {probe?.close(); if (comparison === controller) comparison = undefined;}
          })();
        }, slowWindowMs);
        while (true) {
          const chunk = await active.read();
          if (!chunk) {
            if (!resumable || loaded === route.bytes || active.response.status !== 206) break;
            const source = active.source; active.close();
            active = await open(source, loaded, undefined, new AbortController());
            if (active.response.status === 200) {chunks = []; loaded = 0; progress(0, total, source);}
            continue;
          }
          if (chunk.byteLength) append(chunk as Uint8Array<ArrayBuffer>);
        }
        check();
        if (resumable && loaded !== route.bytes) throw new Error('Resource size mismatch (' + loaded + '/' + route.bytes + ')');
        if (route.sources.length) {preferred.set(url.origin, origin(active.source)); failed.delete(origin(active.source));}
        const headers = new Headers(active.response.headers);
        headers.delete('Content-Encoding'); headers.set('Content-Length', String(loaded));
        if (!explicitRange) headers.delete('Content-Range');
        const result = new Response(new Blob(chunks, {type: headers.get('Content-Type') || 'application/octet-stream'}),
          {status: explicitRange ? active.response.status : 200, headers});
        Object.defineProperty(result, 'url', {value: active.response.url || active.source});
        progress(loaded, total || loaded, active.source); return result;
      } catch (error) {
        if (signal?.aborted) {nextTransfer?.close(); throw aborted(signal);}
        lastError = error;
        if (active) fail(active.source, replacement ? 'slow-download' : error);
        if (replacement) forced = replacement;
        if (!resumable) {chunks = []; loaded = 0;}
      } finally {
        clearInterval(slow); comparison?.abort(); active?.close();
        if (loaded === route.bytes) {nextTransfer?.close(); nextTransfer = undefined;}
      }
    }
    nextTransfer?.close();
    throw lastError;
  };
}
