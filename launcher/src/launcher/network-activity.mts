function nowMs() {
  return globalThis.performance?.now?.() ?? Date.now();
}

type FetchInput = RequestInfo | URL;
type FetchLike = (input: FetchInput, init?: RequestInit) => Promise<Response>;

interface XhrLike {
  responseType: XMLHttpRequestResponseType;
  withCredentials: boolean;
  status: number;
  statusText: string;
  response: unknown;
  onprogress: ((event: ProgressEvent) => void) | null;
  onerror: (() => void) | null;
  ontimeout: (() => void) | null;
  onabort: (() => void) | null;
  onload: (() => void) | null;
  open(method: string, url: string, async: boolean): void;
  setRequestHeader(name: string, value: string): void;
  getResponseHeader(name: string): string | null;
  getAllResponseHeaders(): string;
  send(): void;
  abort(): void;
}

interface NetworkTask {
  id: string;
  url: string;
  title: string;
  label: string;
  kind: string;
  phase: string;
  loaded: number;
  total: number;
  startedAt: number;
  updatedAt: number;
}

export interface NetworkActivitySnapshot {
  readonly active: NetworkTask[];
  readonly count: number;
  readonly loaded: number;
  readonly total: number;
}

interface NetworkTaskPatch {
  url?: unknown;
  title?: unknown;
  label?: unknown;
  kind?: unknown;
  phase?: unknown;
  loaded?: unknown;
  total?: unknown;
}

interface NetworkActivityOptions {
  fetchImpl?: FetchLike;
  xhrFactory?: (() => XhrLike) | null;
  onChange?: (snapshot: NetworkActivitySnapshot) => void;
  clock?: () => number;
  /** Maximum time without a byte-progress advance on the XHR path. */
  xhrTimeoutMs?: number;
  /** Maximum total time for native fetch headers and body consumption. */
  fetchTimeoutMs?: number;
}

function timeoutError(url: string, detail: string) {
  const message = `${url}: ${detail}`;
  if (typeof DOMException === "function") return new DOMException(message, "TimeoutError");
  const error = new Error(message);
  error.name = "TimeoutError";
  return error;
}

function abortError() {
  if (typeof DOMException === "function") return new DOMException("已取消下载", "AbortError");
  const error = new Error("已取消下载");
  error.name = "AbortError";
  return error;
}

function requestUrl(input: FetchInput) {
  if (typeof input === "string") return input;
  if (input instanceof URL) return input.href;
  if (typeof Request !== "undefined" && input instanceof Request) return input.url;
  return String(input || "");
}

function cloneTask(task: NetworkTask): NetworkTask {
  return {
    id: task.id,
    url: task.url,
    title: task.title,
    label: task.label,
    kind: task.kind,
    phase: task.phase,
    loaded: task.loaded,
    total: task.total,
    startedAt: task.startedAt,
    updatedAt: task.updatedAt,
  };
}

export function createNetworkActivityTracker({
  fetchImpl = globalThis.fetch as FetchLike,
  xhrFactory = typeof globalThis.XMLHttpRequest === "function"
    ? () => new globalThis.XMLHttpRequest() as XhrLike
    : null,
  onChange = () => {},
  clock = nowMs,
  xhrTimeoutMs = 30_000,
  fetchTimeoutMs = 120_000,
}: NetworkActivityOptions = {}) {
  if (typeof fetchImpl !== "function") throw new TypeError("fetch implementation is required");
  if (typeof onChange !== "function") throw new TypeError("onChange must be a function");
  const xhrTimeout = Math.max(0, Number(xhrTimeoutMs) || 0);
  const fetchTimeout = Math.max(0, Number(fetchTimeoutMs) || 0);
  let serial = 0;
  const tasks = new Map<string, NetworkTask>();

  const snapshot = (): NetworkActivitySnapshot => {
    const active = [...tasks.values()].map(cloneTask);
    return {
      active,
      count: active.length,
      loaded: active.reduce((sum, task) => sum + task.loaded, 0),
      total: active.every(task => task.total > 0)
        ? active.reduce((sum, task) => sum + task.total, 0)
        : 0,
    };
  };

  const emit = () => onChange(snapshot());

  function begin(meta: NetworkTaskPatch = {}) {
    const id = `net-${Date.now().toString(36)}-${++serial}`;
    const startedAt = clock();
    tasks.set(id, {
      id,
      url: String(meta.url || ""),
      title: String(meta.title || "NETWORK REQUEST..."),
      label: String(meta.label || "SERVER REQUEST"),
      kind: String(meta.kind || "network"),
      phase: String(meta.phase || "requesting"),
      loaded: Math.max(0, Number(meta.loaded) || 0),
      total: Math.max(0, Number(meta.total) || 0),
      startedAt,
      updatedAt: startedAt,
    });
    emit();
    return id;
  }

  function update(id: string, patch: NetworkTaskPatch = {}) {
    const task = tasks.get(id);
    if (!task) return false;
    if (patch.title != null) task.title = String(patch.title);
    if (patch.label != null) task.label = String(patch.label);
    if (patch.kind != null) task.kind = String(patch.kind);
    if (patch.phase != null) task.phase = String(patch.phase);
    if (patch.loaded != null) task.loaded = Math.max(0, Number(patch.loaded) || 0);
    if (patch.total != null) task.total = Math.max(0, Number(patch.total) || 0);
    task.updatedAt = clock();
    emit();
    return true;
  }

  function finish(id: string) {
    if (!tasks.delete(id)) return false;
    emit();
    return true;
  }

  async function trackedFetch(input: FetchInput, init: RequestInit = {}, meta: NetworkTaskPatch = {}) {
    const url = requestUrl(input);
    const id = begin({ ...meta, url });
    let timeoutHandle: ReturnType<typeof setTimeout> | null = null;
    let timeoutReject: ((error: unknown) => void) | null = null;
    let abortReject: ((error: unknown) => void) | null = null;
    let timedOut = false;
    let userAborted = false;
    let settled = false;
    let bodyHandedOff = false;
    const controller = typeof AbortController === "function" ? new AbortController() : null;
    const signal = init.signal ?? (typeof Request !== "undefined" && input instanceof Request ? input.signal : null);
    const fetchInit = controller ? { ...init, signal: controller.signal } : init;
    const timeoutPromise = new Promise<never>((_resolve, reject) => {
      timeoutReject = reject;
      timeoutHandle = setTimeout(() => {
        if (settled || userAborted) return;
        timedOut = true;
        const error = timeoutError(url, `native fetch timed out after ${fetchTimeout} ms`);
        timeoutReject?.(error);
        try { controller?.abort(); } catch {}
        finishTracked();
      }, fetchTimeout);
    });
    const abortPromise = new Promise<never>((_resolve, reject) => {
      abortReject = reject;
    });
    const onAbort = () => {
      if (bodyHandedOff) {
        // The caller owns the native stream now; keep its original signal
        // connected even though generic progress/deadline ownership ended.
        signal?.removeEventListener?.("abort", onAbort);
        controller?.abort();
        return;
      }
      if (settled || userAborted) return;
      userAborted = true;
      const error = abortError();
      abortReject?.(error);
      try { controller?.abort(); } catch {}
      finishTracked();
    };
    const cleanup = () => {
      if (timeoutHandle !== null) {
        clearTimeout(timeoutHandle);
        timeoutHandle = null;
      }
      if (!bodyHandedOff) signal?.removeEventListener?.("abort", onAbort);
    };
    const finishTracked = (loaded?: unknown) => {
      if (settled) return;
      settled = true;
      cleanup();
      if (loaded !== undefined) {
        const total = Math.max(0, Number(responseTotal) || 0);
        const finalLoaded = Math.max(0, Number(loaded) || total || 0);
        update(id, { loaded: finalLoaded, total: total || finalLoaded, phase: "receiving" });
      }
      finish(id);
    };
    let responseTotal = 0;
    try {
      if (signal?.aborted) {
        userAborted = true;
        finishTracked();
        throw abortError();
      }
      signal?.addEventListener?.("abort", onAbort, { once: true });
      const response = await Promise.race([
        Promise.resolve().then(() => fetchImpl(input, fetchInit)),
        timeoutPromise,
        abortPromise,
      ]);
      responseTotal = Math.max(0, Number(response.headers?.get?.("content-length")) || 0);
      update(id, { phase: "receiving", total: responseTotal });

      const method = String(init.method || (typeof Request !== "undefined" && input instanceof Request ? input.method : "GET")).toUpperCase();
      if (!response.ok || method === "HEAD" || response.status === 204 || response.status === 205) {
        finishTracked();
        return response;
      }

      // Preserve the browser's native Response/body object. Re-wrapping a
      // Response.body through getReader() + a second ReadableStream has stalled
      // Android WebView/Via before the caller's json()/blob() consumer receives
      // bytes. Progress ownership follows normal Response consumption methods
      // instead; direct body consumers receive the original stream unchanged.
      const settle = (loaded: unknown) => finishTracked(loaded);
      const consumptionMethods = new Set<PropertyKey>(["arrayBuffer", "blob", "formData", "json", "text", "bytes"]);
      return new Proxy(response, {
        get(target, property) {
          if (property === "body") {
            // Preserve the native body identity. Direct stream consumers do
            // not expose a completion callback, so retain the historical
            // ownership boundary when the body is handed to the caller.
            bodyHandedOff = true;
            settle(responseTotal);
            return Reflect.get(target, property, target);
          }
          const value = Reflect.get(target, property, target) as unknown;
          if (!consumptionMethods.has(property) || typeof value !== "function") {
            return typeof value === "function" ? value.bind(target) : value;
          }
          return async (...args: unknown[]) => {
            try {
              const result = await Promise.race([
                Promise.resolve().then(() => (value as (...args: unknown[]) => Promise<unknown>).apply(target, args)),
                timeoutPromise,
                abortPromise,
              ]);
              const loaded = result instanceof Blob ? result.size
                : result instanceof ArrayBuffer ? result.byteLength
                : ArrayBuffer.isView(result) ? result.byteLength
                : typeof result === "string" ? new TextEncoder().encode(result).byteLength
                : responseTotal;
              settle(loaded);
              return result;
            } catch (error) {
              finishTracked();
              throw error;
            }
          };
        },
      });
    } catch (error) {
      finishTracked();
      if (timedOut) throw timeoutError(url, `native fetch timed out after ${fetchTimeout} ms`);
      if (userAborted) throw abortError();
      throw error;
    }
  }

  function trackedXhrFetch(input: FetchInput, init: RequestInit = {}, meta: NetworkTaskPatch = {}) {
    const method = String(init.method || (typeof Request !== "undefined" && input instanceof Request ? input.method : "GET")).toUpperCase();
    if (typeof xhrFactory !== "function" || !new Set(["GET", "HEAD"]).has(method) || init.body != null) {
      return trackedFetch(input, init, meta);
    }
    const url = requestUrl(input);
    const id = begin({ ...meta, url });
    return new Promise<Response>((resolvePromise, reject) => {
      let xhr: XhrLike | null = null;
      let settled = false;
      const signal = init.signal ?? (typeof Request !== "undefined" && input instanceof Request ? input.signal : null);
      let timeoutHandle: ReturnType<typeof setTimeout> | null = null;
      let lastProgressLoaded = 0;
      const cleanup = () => {
        if (timeoutHandle !== null) {
          clearTimeout(timeoutHandle);
          timeoutHandle = null;
        }
        signal?.removeEventListener?.("abort", abort);
        if (xhr) {
          xhr.onprogress = null;
          xhr.onerror = null;
          xhr.ontimeout = null;
          xhr.onabort = null;
          xhr.onload = null;
        }
      };
      const fail = (error: unknown) => {
        if (settled) return;
        settled = true;
        cleanup();
        finish(id);
        reject(error);
      };
      const fallbackToFetch = (error: unknown) => {
        if (settled) return;
        settled = true;
        cleanup();
        finish(id);
        if (signal?.aborted) {
          reject(abortError());
          return;
        }
        // XHR is used here only to expose live byte progress for large Package
        // downloads. Some Chromium/network-stack combinations can report a
        // transport-level status-0/net::ERR_FAILED for a large blob XHR even
        // though the same resource is readable through native fetch. Preserve
        // XHR as the preferred progress path, but retry that narrow transport
        // failure once through the already-supported native Response path.
        void trackedFetch(input, init, meta).then(resolvePromise, fallbackError => {
          reject(fallbackError && (fallbackError instanceof Error || typeof fallbackError === "object")
            ? fallbackError
            : error);
        });
      };
      const abort = () => {
        if (settled) return;
        try { xhr?.abort(); } catch {}
        fail(abortError());
      };
      const timeout = () => {
        if (settled) return;
        settled = true;
        cleanup();
        try { xhr?.abort(); } catch {}
        finish(id);
        reject(timeoutError(url, `XHR download made no progress for ${xhrTimeout} ms`));
      };
      const armTimeout = () => {
        if (timeoutHandle !== null) clearTimeout(timeoutHandle);
        timeoutHandle = setTimeout(timeout, xhrTimeout);
      };
      if (signal?.aborted) {
        fail(abortError());
        return;
      }
      try {
        xhr = xhrFactory();
        const request = xhr;
        if (!request) throw new TypeError("XHR factory returned no request");
        request.open(method, url, true);
        request.responseType = "blob";
        request.withCredentials = init.credentials === "include";
        if (init.headers) {
          const headers = new Headers(init.headers);
          headers.forEach((value, name) => request.setRequestHeader(name, value));
        }
        request.onprogress = event => update(id, {
          phase: "receiving",
          loaded: event.loaded,
          total: event.lengthComputable ? event.total : 0,
        });
        const progress = request.onprogress;
        request.onprogress = event => {
          progress?.(event);
          const loaded = Math.max(0, Number(event.loaded) || 0);
          if (loaded > lastProgressLoaded) {
            lastProgressLoaded = loaded;
            armTimeout();
          }
        };
        request.onerror = () => fallbackToFetch(new TypeError(`${url}: network request failed`));
        request.ontimeout = () => fallbackToFetch(new TypeError(`${url}: network request timed out`));
        request.onabort = () => fail(abortError());
        request.onload = () => {
          if (settled) return;
          if (request.status === 0) {
            fallbackToFetch(new TypeError(`${url}: network request returned status 0`));
            return;
          }
          const blob = method === "HEAD" || [204, 205, 304].includes(request.status)
            ? null : request.response instanceof Blob ? request.response : null;
          const loaded = blob instanceof Blob ? blob.size : 0;
          const total = Math.max(0, Number(request.getResponseHeader?.("content-length")) || loaded);
          try {
            const headers = new Headers();
            for (const line of String(request.getAllResponseHeaders?.() || "").trim().split(/[\r\n]+/)) {
              const split = line.indexOf(":");
              if (split > 0) headers.append(line.slice(0, split).trim(), line.slice(split + 1).trim());
            }
            const response = new Response(blob, { status: request.status, statusText: request.statusText, headers });
            settled = true;
            cleanup();
            update(id, { phase: "receiving", loaded, total });
            finish(id);
            resolvePromise(response);
          } catch (error) {
            fail(error);
          }
        };
        signal?.addEventListener?.("abort", abort, { once: true });
        armTimeout();
        request.send();
      } catch (error) {
        fail(error);
      }
    });
  }

  return {
    begin,
    update,
    finish,
    fetch: trackedFetch,
    xhrFetch: trackedXhrFetch,
    snapshot,
    get activeCount() { return tasks.size; },
  };
}
