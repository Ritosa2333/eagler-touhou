import {
  RUNTIME_MANIFEST_FILE, RUNTIME_CAPABILITIES, RUNTIME_PREPARE, RUNTIME_CACHE_PROTOCOL,
  RUNTIME_CACHE_MAX_PREVIOUS, canonicalRuntimePayload, findRuntimeGroup,
  runtimeGenerationBase, runtimeGenerationEntry, validateRuntimeManifest, parseRuntimeGenerationPath,
  type RuntimeManifest,
} from "../contracts/runtime-generations.mjs";

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
async function message(worker: WorkerLike, data: unknown, timeout: number): Promise<Record<string, unknown> | null> {
  const channel = new MessageChannel();
  try {
    return await bounded(new Promise<Record<string, unknown> | null>(resolve => {
      channel.port1.onmessage = event => resolve(event.data as Record<string, unknown>);
      worker.postMessage(data, [channel.port2]);
    }), timeout, null);
  } finally { channel.port1.close(); channel.port2.close(); }
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
/** Complete local programs are selected without a network barrier. A deferred
 * check may stage a different complete generation; it never redirects this
 * launch or reports an update whose bytes failed verification. */
export async function prepareRuntimeLaunch(runtime: string, {
  baseUrl = new URL("./", globalThis.location.href).href,
  fetchImpl = globalThis.fetch.bind(globalThis), worker, exclude = [], timeoutMs = 8000,
}: LaunchOptions = {}): Promise<PreparedRuntime> {
  const base = new URL("./", baseUrl);
  const requested = new URL(runtime, base);
  if (requested.origin !== base.origin || !requested.pathname.startsWith(base.pathname)) {
    throw new Error("Published Runtime entry is outside the Launcher mount");
  }
  const path = requested.pathname.slice(base.pathname.length);
  if (exclude.length > RUNTIME_CACHE_MAX_PREVIOUS + 1 || exclude.some(id => !/^[a-f0-9]{64}$/.test(id))) {
    throw new Error("Invalid Runtime fallback exclusions");
  }
  async function network<T>(url: string, read: (response: Response, progress: () => void) => Promise<T>, retries = 0): Promise<T> {
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
        const response = await fetchImpl(url, { cache: "no-store", redirect: font ? "follow" : "error", signal: controller.signal });
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
  const target = await bounded(Promise.resolve(worker === undefined ? runtimeProgramWorker(base.href) : worker), 5000, null);
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
  if (target) {
    const capability = await message(target, { type: RUNTIME_CAPABILITIES }, 2000);
    capable = capability?.ok === true && capability.protocol === RUNTIME_CACHE_PROTOCOL;
    if (capable && capability?.localOnly === true) {
      const local = prepared(await message(target, {type:RUNTIME_PREPARE,entry:path,localOnly:true,exclude:[...exclude]}, 120000), null);
      if (local) {
        let checking: Promise<RuntimeUpdate | null> | undefined;
        local.checkForUpdate = () => checking ??= (async () => {
          const manifest = await latest(), group = manifest && findRuntimeGroup(manifest, path);
          if (!group || group.current.generation === local.generation || exclude.includes(group.current.generation)) return null;
          const update = prepared(await message(target, {type:RUNTIME_PREPARE,entry:path,catalog:manifest,exclude:[...exclude]},120000),manifest);
          return update?.cached && update.generation === group.current.generation ? {url:update.url,generation:update.generation} : null;
        })().catch(() => null);
        return local;
      }
    }
  }
  const manifest = await latest();
  if (target && capable) {
    const result = prepared(await message(target, {type:RUNTIME_PREPARE,entry:path,
      ...(manifest ? {catalog:manifest} : {}),exclude:[...exclude]},120000),manifest);
    if (result) return result;
    // Storage restrictions must not prevent a verified ordinary HTTP launch.
  }
  const group = manifest && findRuntimeGroup(manifest, path);
  if (!group) throw new Error("Runtime is unavailable online and no complete offline copy was prepared");
  let lastError: unknown;
  for (const descriptor of [group.current, ...group.previous]) {
    if (exclude.includes(descriptor.generation)) continue;
    try {
      const actual = await digest(new TextEncoder().encode(canonicalRuntimePayload(descriptor.entry, descriptor.files)).buffer);
      if (actual !== descriptor.generation) throw new Error("Runtime generation identity mismatch");
      const directory = runtimeGenerationBase(group.root, descriptor.generation);
      // No cache authority is available. Verify this complete set first; normal
      // subsequent HTTP requests still address immutable, never latest, files.
      for (const file of descriptor.files) await network(new URL(directory + file.path, base).href, async (response, progress) => {
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
      }, 2);
      const selected = new URL(runtimeGenerationEntry(group.root, descriptor), base);
      selected.search = requested.search; selected.searchParams.delete("v"); selected.hash = requested.hash;
      return { url: selected.href, generation: descriptor.generation, cached: false };
    } catch (error) { lastError = error; }
  }
  throw lastError || new Error("No complete Runtime is available");
}
