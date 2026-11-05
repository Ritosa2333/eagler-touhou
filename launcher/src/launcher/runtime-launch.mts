import {
  RUNTIME_MANIFEST_FILE, RUNTIME_CAPABILITIES, RUNTIME_PREPARE, RUNTIME_PROGRESS, RUNTIME_IMPORT, RUNTIME_CACHE_PROTOCOL,
  RUNTIME_CACHE_MAX_PREVIOUS, canonicalRuntimePayload, findRuntimeGroup,
  runtimeGenerationBase, runtimeGenerationEntry, validateRuntimeManifest, parseRuntimeGenerationPath,
  parseRuntimeDownloadProgress, type RuntimeDownloadProgress, type RuntimeManifest,
} from "../contracts/runtime-generations.mjs";
import {consumeNetworkBody} from './network-activity.mjs';

export interface PreparedRuntime {
  url: string;
  generation: string;
  cached: boolean;
  checkForUpdate?: () => Promise<RuntimeUpdate | null>;
}
export interface RuntimeUpdate {url: string; generation: string;}
interface WorkerLike { postMessage(message: unknown, transfer: Transferable[]): void; }
interface LaunchOptions {
  baseUrl?: string;
  fetchImpl?: typeof fetch;
  worker?: WorkerLike | null | PromiseLike<WorkerLike | null>;
  exclude?: readonly string[];
  timeoutMs?: number;
  /** Online multiplayer must verify the current publication before navigation. */
  requireCurrent?: boolean;
  expectedGeneration?: string;
  onProgress?: (progress: RuntimeDownloadProgress) => void;
}
async function digest(bytes: ArrayBuffer): Promise<string> {
  return Array.from(new Uint8Array(await crypto.subtle.digest("SHA-256", bytes)),
    byte => byte.toString(16).padStart(2, "0")).join("");
}
async function bounded<T>(task: PromiseLike<T>, timeout: number, fallback: T): Promise<T> {
  let timer: ReturnType<typeof setTimeout> | undefined;
  try {
    return await Promise.race([Promise.resolve(task).catch(() => fallback), new Promise<T>(resolve => {
      timer = setTimeout(() => resolve(fallback), timeout);
    })]);
  } finally { clearTimeout(timer); }
}
async function message(worker: WorkerLike, data: unknown, timeout: number, onProgress?: LaunchOptions['onProgress'], transfer: Transferable[] = []): Promise<Record<string, unknown> | null> {
  const channel = new MessageChannel();
  const active = new Map<string, RuntimeDownloadProgress>();
  try {
    return await bounded(new Promise<Record<string, unknown> | null>(resolve => {
      channel.port1.onmessage = event => {
        if (event.data?.type === RUNTIME_PROGRESS) {
          const progress = parseRuntimeDownloadProgress(event.data);
          if (!progress) return;
          if (progress.phase === 'complete' || progress.phase === 'failed') active.delete(progress.path);
          else active.set(progress.path, progress);
          onProgress?.(progress); return;
        }
        resolve(event.data as Record<string, unknown>);
      };
      worker.postMessage({...data as object, ...(onProgress ? {progress: true} : {})}, [channel.port2, ...transfer]);
    }), timeout, null);
  } finally {
    channel.port1.close(); channel.port2.close();
    for (const progress of active.values()) onProgress?.({...progress, phase: 'failed'});
  }
}
export async function runtimeProgramWorker(base: string): Promise<WorkerLike | null> {
  try {
    const serviceWorker = navigator.serviceWorker;
    if (!serviceWorker) return null;
    const scope = new URL('runtime/', base).href;
    const existing = await serviceWorker.getRegistration(scope);
    if (existing?.scope === scope && existing.active) return existing.active;
    const registration = await serviceWorker.register(new URL('runtime-program-sw.js', base), {scope, updateViaCache:'none'});
    if (registration.active) return registration.active;
    const installing = registration.installing || registration.waiting;
    if (!installing) return null;
    return await bounded(new Promise<ServiceWorker | null>(resolve => {
      const changed = () => {
        if (installing.state !== 'activated' && installing.state !== 'redundant') return;
        installing.removeEventListener('statechange', changed);
        resolve(installing.state === 'activated' ? installing : null);
      };
      installing.addEventListener('statechange', changed);changed();
    }), 5000, null);
  } catch { return null; }
}
/** Ordinary local programs are selected without a network barrier. A deferred
 * check may stage a different complete generation; it never redirects this
 * launch or reports an update whose bytes failed verification. Online MP uses
 * requireCurrent and optionally a room agreement, with no old-program fallback. */
export async function prepareRuntimeLaunch(runtime: string, {
  baseUrl = new URL("./", globalThis.location.href).href,
  fetchImpl = globalThis.fetch.bind(globalThis), worker, exclude = [], timeoutMs = 8000,
  requireCurrent = false, expectedGeneration, onProgress,
}: LaunchOptions = {}): Promise<PreparedRuntime> {
  if (expectedGeneration !== undefined && !/^[a-f0-9]{64}$/.test(expectedGeneration)) throw new Error('Invalid agreed Runtime generation');
  requireCurrent ||= expectedGeneration !== undefined;
  const base = new URL("./", baseUrl);
  const requested = new URL(runtime, base);
  if (requested.origin !== base.origin || !requested.pathname.startsWith(base.pathname)) {
    throw new Error("Published Runtime entry is outside the Launcher mount");
  }
  const path = requested.pathname.slice(base.pathname.length);
  if (exclude.length > RUNTIME_CACHE_MAX_PREVIOUS + 1 || exclude.some(id => !/^[a-f0-9]{64}$/.test(id))) {
    throw new Error("Invalid Runtime fallback exclusions");
  }
  async function network<T>(url: string, read: (response: Response, progress: () => void) => Promise<T>, retries = 0, cache: RequestCache = 'no-store'): Promise<T> {
    let lastError: unknown;
    for (let attempt = 0; attempt <= retries; attempt++) {
      const controller = new AbortController();
      let timer: ReturnType<typeof setTimeout> | undefined;
      const progress = () => {
        clearTimeout(timer);
        timer = setTimeout(() => controller.abort(new Error(`Runtime download stalled for ${timeoutMs} ms: ${url}`)), timeoutMs);
      };
      progress();
      try {
        // Data-only fonts may live on resource nodes. Every byte is still
        // checked against the generation's declared size and SHA-256 below.
        const font = /\/fonts\/[^/]+\.(?:bin|ttf|ttc|otf|woff2?)$/.test(new URL(url).pathname);
        const response = await fetchImpl(url, { cache, redirect: font ? "follow" : "error", signal: controller.signal });
        if (!response.ok) throw new Error(`Runtime HTTP ${response.status}: ${url}`);
        progress();
        return await read(response, progress);
      } catch (error) {
        if (!controller.signal.aborted && !(error instanceof TypeError)) throw error;
        lastError = error;
      } finally { clearTimeout(timer); }
      if (attempt < retries) await new Promise(resolve => setTimeout(resolve, 350 * (attempt + 1)));
    }
    const reason = lastError instanceof Error ? lastError.message : String(lastError);
    throw new Error(`Runtime request failed after ${retries + 1} attempts: ${url} (${reason})`);
  }
  const latest = (): Promise<RuntimeManifest | null> => network(new URL(RUNTIME_MANIFEST_FILE, base).href, async response => {
    const text = await response.text();
    if (text.length > 4 * 1024 * 1024) throw new Error("Runtime Manifest exceeds size limit");
    return validateRuntimeManifest(JSON.parse(text));
  }).catch(() => null);
  const workerTask = Promise.resolve(worker === undefined ? runtimeProgramWorker(base.href) : worker);
  const target = await bounded(workerTask, 5000, null);
  const requestedRoot = parseRuntimeGenerationPath(path)?.root || path.slice(0, path.lastIndexOf('/') + 1);
  const reportProgress: LaunchOptions['onProgress'] = onProgress ? progress => {
    if (progress.path === RUNTIME_MANIFEST_FILE || parseRuntimeGenerationPath(progress.path)?.root === requestedRoot) onProgress(progress);
  } : undefined;
  function prepared(result: Record<string, unknown> | null, manifest: RuntimeManifest | null): PreparedRuntime | null {
    if (!result?.ok || result.protocol !== RUNTIME_CACHE_PROTOCOL || typeof result.entry !== 'string' || typeof result.generation !== 'string') return null;
    const selected = new URL(result.entry, base);
    const identity = parseRuntimeGenerationPath(selected.pathname.slice(base.pathname.length));
    const requestedRoot = parseRuntimeGenerationPath(path)?.root || (manifest && findRuntimeGroup(manifest, path)?.root) || path.slice(0, path.lastIndexOf('/') + 1);
    if (selected.origin !== base.origin || !selected.pathname.startsWith(base.pathname) ||
        !identity || identity.generation !== result.generation || identity.root !== requestedRoot ||
        !identity.file.endsWith('.html') || exclude.includes(result.generation)) throw new Error('Invalid Runtime preparation response');
    selected.search = requested.search;selected.searchParams.delete('v');selected.hash = requested.hash;
    return {url:selected.href, generation:result.generation, cached:result.cached === true};
  }
  let capable = false;
  let importCapable = false;
  if (target) {
    const capability = await message(target, { type: RUNTIME_CAPABILITIES }, 2000);
    capable = capability?.ok === true && capability.protocol === RUNTIME_CACHE_PROTOCOL;
    importCapable = capable && capability?.importFiles === true;
    if (!requireCurrent && capable && capability?.localOnly === true) {
      const local = prepared(await message(target, {type:RUNTIME_PREPARE,entry:path,localOnly:true,exclude:[...exclude]}, 120000, reportProgress), null);
      if (local) {
        let checking: Promise<RuntimeUpdate | null> | undefined;
        local.checkForUpdate = () => checking ??= (async () => {
          const manifest = await latest(), group = manifest && findRuntimeGroup(manifest, path);
          if (!group || group.current.generation === local.generation || exclude.includes(group.current.generation)) return null;
          const update = prepared(await message(target, {type:RUNTIME_PREPARE,entry:path,catalog:manifest,exclude:[...exclude]},120000,reportProgress),manifest);
          return update?.cached && update.generation === group.current.generation ? {url:update.url,generation:update.generation} : null;
        })().catch(() => null);
        return local;
      }
    }
  }
  const manifest = await latest();
  const group = manifest && findRuntimeGroup(manifest, path);
  if (requireCurrent && (!group || exclude.includes(group.current.generation))) {
    throw new Error('联机程序版本检查失败，请联网并重试准备资源。');
  }
  if (expectedGeneration && group?.current.generation !== expectedGeneration) {
    throw new Error('联机程序已有更新，请返回房间，双方重新准备资源后再开始。');
  }
  if (target && capable) {
    const result = prepared(await message(target, {type:RUNTIME_PREPARE,entry:path,
      ...(manifest ? {catalog:manifest} : {}),exclude:[...exclude]},120000,reportProgress),manifest);
    if (result && (!requireCurrent || result.generation === group!.current.generation)) return result;
    // Storage restrictions must not prevent a verified ordinary HTTP launch.
  }
  if (!group) throw new Error("Runtime is unavailable online and no complete offline copy was prepared");
  let lastError: unknown;
  for (const descriptor of requireCurrent ? [group.current] : [group.current, ...group.previous]) {
    if (exclude.includes(descriptor.generation)) continue;
    try {
      const actual = await digest(new TextEncoder().encode(canonicalRuntimePayload(descriptor.entry, descriptor.files)).buffer);
      if (actual !== descriptor.generation) throw new Error("Runtime generation identity mismatch");
      const directory = runtimeGenerationBase(group.root, descriptor.generation);
      const verified: {path: string; buffer: ArrayBuffer}[] = [];
      // Verify the complete set, and populate the ordinary HTTP cache for the
      // subsequent immutable iframe requests (including a late-starting SW).
      for (const file of descriptor.files) await network(new URL(directory + file.path, base).href, (response, progress) => consumeNetworkBody(response, async received => {
        received(0, file.bytes);
        // Bound inactivity, not the total transfer time of a large WASM.
        const reader = response.body?.getReader();
        let bytes: ArrayBuffer;
        if (!reader) bytes = await response.arrayBuffer();
        else {
          const chunks: Uint8Array[] = [];
          let length = 0;
          try {
            while (true) {
              const { done, value } = await reader.read();
              if (done) break;
              if (value.byteLength) progress();
              length += value.byteLength;
              received(length, file.bytes);
              if (length > file.bytes) throw new Error(`Runtime integrity mismatch: ${file.path}`);
              chunks.push(value);
            }
          } finally { await reader.cancel().catch(() => {}); reader.releaseLock(); }
          const combined = new Uint8Array(length);
          let offset = 0;
          for (const chunk of chunks) { combined.set(chunk, offset); offset += chunk.byteLength; }
          bytes = combined.buffer;
        }
        if (bytes.byteLength !== file.bytes || await digest(bytes) !== file.sha256) throw new Error(`Runtime integrity mismatch: ${file.path}`);
        verified.push({path: file.path, buffer: bytes});
      }), 2, 'reload');
      // The App Shell may finish installing while HTTP verification runs.
      // Transfer the checked bytes into the canonical atomic cache rather than
      // asking that worker/iframe to download them again. HTTP caching remains
      // a best-effort fallback when no cache authority/storage is available.
      const lateWorker = target ?? await bounded(workerTask, 1000, null);
      let canImport = importCapable;
      if (lateWorker && lateWorker !== target) {
        const capability = await message(lateWorker, {type: RUNTIME_CAPABILITIES}, 2000);
        canImport = capability?.ok === true && capability.protocol === RUNTIME_CACHE_PROTOCOL && capability.importFiles === true;
      }
      if (lateWorker && canImport) {
        try {
          const imported = prepared(await message(lateWorker, {type:RUNTIME_IMPORT,root:group.root,descriptor,files:verified},120000,
            undefined,verified.map(file=>file.buffer)),manifest);
          if (imported?.cached && imported.generation === descriptor.generation) return imported;
        } catch { /* Optional cache handoff must not prevent the verified HTTP launch. */ }
      }
      const selected = new URL(runtimeGenerationEntry(group.root, descriptor), base);
      selected.search = requested.search; selected.searchParams.delete("v"); selected.hash = requested.hash;
      return { url: selected.href, generation: descriptor.generation, cached: false };
    } catch (error) { lastError = error; }
  }
  throw lastError || new Error("No complete Runtime is available");
}
