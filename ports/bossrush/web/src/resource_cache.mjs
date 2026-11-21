/*
 * BRO Web Runtime -- 可增量更新的资源缓存层
 *
 * 设计目标（清酒 2026-11-21）：
 *   - 当前分发形态是「导入包」，运行时形态仍是逐文件 HTTP fetch；
 *   - 后续接线上分发时，不能每次修 bug 都让玩家重下 138MB；
 *   - 因此版本真相放在 manifest（逐文件 sha256），缓存按内容寻址：
 *       1. 新版本只拉 h 变化的 assets；
 *       2. 未变文件命中同一 hash 缓存，零下载；
 *       3. WASM / 代码 / 素材各自有独立版本，脚本修复只换 3.3MB WASM；
 *       4. 导入包经 resources 命令送来的 blob 也按 hash 进同一缓存。
 *
 * 不复制任何 138MB 资源；本模块只存 manifest 和小额索引。
 */

export const RESOURCE_CACHE_SCHEMA = 'bro/resource-cache/1';
export const PACKAGE_MANIFEST_SCHEMA = 'bro/package-manifest/1';

const ACTIVE_KEY = 'BRO.resource.active.v1';
const PREVIOUS_KEY = 'BRO.resource.previous.v1';
const PATHS_KEY = 'BRO.resource.paths.v1';
const HASH_RE = /^[0-9a-f]{64}$/;

export function normalizeAssetPath(value) {
  return String(value || '')
    .replace(/\\/g, '/')
    .replace(/^\.\//, '')
    .replace(/^\/+/, '')
    .replace(/^game\//i, '')
    .split('/')
    .filter((part) => part && part !== '.')
    .join('/');
}

const pathKey = (value) => normalizeAssetPath(value).toLowerCase();

export function validatePackageManifest(value) {
  if (!value || value.schema !== PACKAGE_MANIFEST_SCHEMA || !Array.isArray(value.files)) {
    throw new Error('invalid package manifest');
  }
  const files = [];
  for (const raw of value.files) {
    if (!raw || typeof raw.p !== 'string' || typeof raw.h !== 'string') continue;
    const hash = String(raw.h).toLowerCase();
    if (!HASH_RE.test(hash)) continue;
    files.push({
      ...raw,
      p: normalizeAssetPath(raw.p),
      h: hash,
      n: Number.isFinite(Number(raw.n)) ? Math.max(0, Math.round(Number(raw.n))) : 0,
    });
  }
  return { ...value, files };
}

/** 纯函数：对比新旧 manifest，得到真正需要下载的文件集合。 */
export function planManifestUpdate(next, current) {
  const target = validatePackageManifest(next);
  const base = current && Array.isArray(current.files) ? current : { files: [] };
  const byPath = new Map();
  for (const file of base.files) {
    if (file && typeof file.p === 'string') byPath.set(pathKey(file.p), file);
  }
  const nextPaths = new Set(target.files.map((file) => pathKey(file.p)));
  const added = [];
  const changed = [];
  for (const file of target.files) {
    const old = byPath.get(pathKey(file.p));
    if (!old) added.push(file);
    else if (String(old.h).toLowerCase() !== file.h || Number(old.n) !== file.n) changed.push(file);
  }
  const removed = base.files.filter((file) => file && typeof file.p === 'string' && !nextPaths.has(pathKey(file.p)));
  const downloads = added.concat(changed);
  return {
    added,
    changed,
    removed,
    downloads,
    unchanged: target.files.length - downloads.length,
    downloadBytes: downloads.reduce((sum, file) => sum + (Number(file.n) || 0), 0),
    version: target.version,
    previousVersion: base.version || null,
  };
}

function readJson(store, key, fallback) {
  try {
    const text = store && store.getItem(key);
    return text ? JSON.parse(text) : fallback;
  } catch { return fallback; }
}

function writeJson(store, key, value) {
  try { if (store && store.setItem) store.setItem(key, JSON.stringify(value)); }
  catch { /* 隐私模式或配额不足：缓存降级为本次会话内有效 */ }
}

function joinUrl(base, path) {
  const root = String(base || '/game/').replace(/\/?$/, '/');
  return root + normalizeAssetPath(path).split('/').map(encodeURIComponent).join('/');
}

function toUint8(bytes) {
  if (bytes instanceof Uint8Array) return bytes;
  if (bytes instanceof ArrayBuffer) return new Uint8Array(bytes);
  if (bytes && bytes.buffer) return new Uint8Array(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  return new Uint8Array(0);
}

async function defaultHash(bytes) {
  const webCrypto = globalThis.crypto && globalThis.crypto.subtle;
  if (!webCrypto) return null;
  const u8 = toUint8(bytes);
  const copy = u8.slice().buffer;
  const digest = await webCrypto.digest('SHA-256', copy);
  return Array.from(new Uint8Array(digest)).map((n) => n.toString(16).padStart(2, '0')).join('');
}

/**
 * @param {object} opts
 * @param {string} [opts.assetBase]        游戏资源 HTTP 根，默认 /game/
 * @param {string} [opts.manifestUrl]      package-manifest.json URL
 * @param {string} [opts.cacheName]        Cache API 命名空间
 * @param {object} [opts.storage]          localStorage 兼容对象（可注入）
 * @param {object} [opts.caches]           Cache API（可注入，便于单测）
 * @param {function} [opts.fetchImpl]      fetch 实现（可注入）
 * @param {function} [opts.hashImpl]       sha256(bytes) => hex（可注入）
 */
export function createResourceCache(opts = {}) {
  const assetBase = String(opts.assetBase || '/game/');
  // manifestUrl 可变：宿主可在 configure 阶段下发线上清单地址，运行时切到线上增量更新。
  let manifestUrl = String(opts.manifestUrl || '/dist/package-manifest.json');
  const cacheName = String(opts.cacheName || 'BossRush-assets-v1');
  const store = opts.storage !== undefined ? opts.storage : globalThis.localStorage;
  const cacheApi = opts.caches !== undefined ? opts.caches : globalThis.caches;
  const fetchImpl = opts.fetchImpl || ((...args) => fetch(...args));
  const hashImpl = opts.hashImpl || defaultHash;
  let cachePromise = null;

  let active = readJson(store, ACTIVE_KEY, null);
  let previous = readJson(store, PREVIOUS_KEY, null);
  let overrides = readJson(store, PATHS_KEY, {});
  if (active) { try { active = validatePackageManifest(active); } catch { active = null; } }
  if (previous) { try { previous = validatePackageManifest(previous); } catch { previous = null; } }
  if (!overrides || typeof overrides !== 'object' || Array.isArray(overrides)) overrides = {};

  const fileIndex = new Map();
  const overrideIndex = new Map();
  const blobUrls = new Map();
  const reindex = () => {
    fileIndex.clear();
    for (const file of active?.files || []) fileIndex.set(pathKey(file.p), file);
    overrideIndex.clear();
    for (const [key, value] of Object.entries(overrides)) {
      if (value && HASH_RE.test(String(value.h).toLowerCase())) overrideIndex.set(key, value);
    }
  };
  reindex();

  const cache = async () => {
    if (!cacheApi || typeof cacheApi.open !== 'function') return null;
    if (!cachePromise) cachePromise = cacheApi.open(cacheName);
    return cachePromise;
  };
  const blobRequest = (hash) => `https://bro.invalid/blob/${hash}`;
  const hashFromRequest = (request) => {
    const url = typeof request === 'string' ? request : request?.url || '';
    const match = /\/([0-9a-f]{64})$/.exec(url);
    return match ? match[1] : null;
  };

  async function getBlob(hash) {
    const c = await cache();
    if (!c) return null;
    const res = await c.match(blobRequest(hash));
    if (!res || !res.ok) return null;
    const bytes = toUint8(await res.arrayBuffer());
    const declared = Number(res.headers?.get?.('X-BRO-SIZE'));
    if (Number.isFinite(declared) && declared > 0 && declared !== bytes.byteLength) return null;
    return bytes;
  }

  async function putBlob(hash, bytes, size) {
    const c = await cache();
    if (!c) return false;
    const u8 = toUint8(bytes);
    if (Number.isFinite(size) && size > 0 && size !== u8.byteLength) {
      throw new Error(`resource size mismatch: ${size}/${u8.byteLength}`);
    }
    const response = new Response(u8.slice(), {
      status: 200,
      headers: {
        'Content-Type': 'application/octet-stream',
        'X-BRO-SHA256': hash,
        'X-BRO-SIZE': String(u8.byteLength),
      },
    });
    await c.put(blobRequest(hash), response);
    return true;
  }

  async function verify(bytes, expected) {
    if (!expected || typeof hashImpl !== 'function') return;
    const actual = await hashImpl(bytes);
    if (actual && String(actual).toLowerCase() !== String(expected).toLowerCase()) {
      throw new Error(`resource hash mismatch: expected ${expected}, got ${actual}`);
    }
  }

  function resolveFile(path) {
    const key = pathKey(path);
    return overrideIndex.get(key) || fileIndex.get(key) || null;
  }

  function remoteUrl(file) {
    if (!file) return '';
    const hash = String(file.h || '').toLowerCase();
    const base = joinUrl(assetBase, file.p);
    return HASH_RE.test(hash) ? `${base}?h=${hash.slice(0, 12)}` : base;
  }

  async function ensureBlob(file, onProgress) {
    if (!file) return null;
    const hash = String(file.h || '').toLowerCase();
    if (!HASH_RE.test(hash)) return null;
    const cached = await getBlob(hash);
    if (cached) return cached;
    const res = await fetchImpl(remoteUrl(file));
    if (!res || !res.ok) throw new Error(`resource fetch failed: ${file.p} (${res?.status ?? 'no response'})`);
    const bytes = toUint8(await res.arrayBuffer());
    await verify(bytes, hash);
    await putBlob(hash, bytes, file.n);
    onProgress?.(file, bytes.byteLength);
    return bytes;
  }

  async function url(path) {
    const file = resolveFile(path);
    if (!file) return joinUrl(assetBase, path);
    const hash = String(file.h || '').toLowerCase();
    if (!blobUrls.has(hash)) {
      const bytes = await ensureBlob(file);
      if (bytes && typeof URL !== 'undefined' && typeof URL.createObjectURL === 'function') {
        blobUrls.set(hash, URL.createObjectURL(new Blob([bytes.slice()], { type: 'application/octet-stream' })));
      }
    }
    return blobUrls.get(hash) || remoteUrl(file);
  }

  async function fetchAsset(path) {
    const file = resolveFile(path);
    if (file) {
      const bytes = await ensureBlob(file);
      if (bytes) {
        return new Response(bytes.slice(), {
          status: 200,
          headers: { 'Content-Type': 'application/octet-stream', 'X-BRO-CACHE': 'hit' },
        });
      }
    }
    return fetchImpl(joinUrl(assetBase, path));
  }

  async function refreshManifest(options = {}) {
    const target = String(options.manifestUrl || manifestUrl);
    const urlWithCacheBust = `${target}${target.includes('?') ? '&' : '?'}t=${Date.now()}`;
    const res = await fetchImpl(urlWithCacheBust, { cache: 'no-store' });
    if (!res || !res.ok) throw new Error(`manifest fetch failed: ${res?.status ?? 'no response'}`);
    const manifest = validatePackageManifest(await res.json());
    // 保持纯语义：只算差集、不提交；提交由 syncOnline / commitManifest 显式完成。
    return { manifest, plan: planManifestUpdate(manifest, active) };
  }

  /** 设置线上清单地址（configure 阶段下发）。返回当前生效地址。 */
  function setManifestUrl(url) {
    if (typeof url === 'string' && url.trim()) manifestUrl = url.trim();
    return manifestUrl;
  }

  /**
   * 一站式线上增量同步：拉清单 → 算差集 → 只下载变化文件 → 提交新清单。
   * 语义：
   *   - 线上清单不可达（导入包形态 / 离线）：返回 {status:'offline'}，绝不抛出，
   *     调用方据此静默降级到导入包缓存或本地 HTTP 资源，不阻塞游戏启动。
   *   - 清单一致：{status:'current'}，零下载。
   *   - 有变化：{status:'updated'}，prefetch 只取 downloads，未变文件复用缓存。
   */
  async function syncOnline(options = {}) {
    let refreshed;
    try {
      refreshed = await refreshManifest({ manifestUrl: options.manifestUrl, commit: false });
    } catch (error) {
      return { status: 'offline', error: String(error && error.message || error), manifestUrl };
    }
    const { manifest, plan } = refreshed;
    if (!plan.downloads.length) {
      commitManifest(manifest);
      return { status: 'current', version: manifest.version, previousVersion: plan.previousVersion,
        files: manifest.files.length, downloads: 0, downloadBytes: 0, manifestUrl };
    }
    let fetched = { downloaded: 0, bytes: 0 };
    try {
      fetched = await prefetch(plan, { manifest, onProgress: options.onProgress });
    } catch (error) {
      // 清单拿到了但资源拉取失败：保留旧 active，不提交半更新状态。
      return { status: 'offline', version: manifest.version,
        error: String(error && error.message || error), manifestUrl };
    }
    return { status: 'updated', version: manifest.version, previousVersion: plan.previousVersion,
      files: manifest.files.length, downloads: plan.downloads.length,
      downloadBytes: plan.downloadBytes, ...fetched, manifestUrl };
  }

  function commitManifest(manifest) {
    const next = validatePackageManifest(manifest);
    previous = active;
    active = next;
    writeJson(store, ACTIVE_KEY, active);
    writeJson(store, PREVIOUS_KEY, previous);
    reindex();
    return { version: active.version, previousVersion: previous?.version || null };
  }

  /** 只预取指定文件（默认全部变化项），用于在线分发或手动“更新资源”。 */
  async function prefetch(plan, options = {}) {
    const files = Array.isArray(options.files) ? options.files : (plan?.downloads || []);
    const limit = Math.max(1, Math.min(8, Number(options.concurrency) || 4));
    let cursor = 0;
    let downloaded = 0;
    let bytes = 0;
    let next = 0;
    const workers = Array.from({ length: Math.min(limit, files.length) }, async () => {
      while (next < files.length) {
        const file = files[next++];
        await ensureBlob(file, (_, size) => {
          downloaded++; bytes += size;
          options.onProgress?.({ downloaded, bytes, total: files.length, path: file.p });
        });
      }
    });
    await Promise.all(workers);
    if (plan?.manifest || options.manifest) commitManifest(options.manifest || plan.manifest);
    return { downloaded, bytes };
  }

  /** eagler launcher 通过 resources 命令送来的导入包文件；blob URL 很快会被宿主 revoke，必须当场读。 */
  async function installResources(resources) {
    const stats = { installed: 0, bytes: 0, skipped: 0, errors: [] };
    for (const item of Array.isArray(resources) ? resources : []) {
      const path = normalizeAssetPath(item?.path || item?.target || item?.p);
      if (!path || !item?.url) { stats.skipped++; continue; }
      try {
        const res = await fetchImpl(item.url);
        if (!res || !res.ok) throw new Error(`HTTP ${res?.status ?? 'no response'}`);
        const bytes = toUint8(await res.arrayBuffer());
        let hash = String(item.sha256 || item.h || '').toLowerCase();
        if (!HASH_RE.test(hash)) hash = await hashImpl(bytes) || '';
        if (!HASH_RE.test(hash)) throw new Error('resource hash unavailable');
        await verify(bytes, hash);
        await putBlob(hash, bytes, item.bytes ?? bytes.byteLength);
        overrides[pathKey(path)] = { p: path, h: hash, n: bytes.byteLength };
        stats.installed++;
        stats.bytes += bytes.byteLength;
      } catch (error) {
        stats.errors.push(`${path}: ${error && error.message ? error.message : String(error)}`);
      }
    }
    writeJson(store, PATHS_KEY, overrides);
    reindex();
    return stats;
  }

  async function gc() {
    const c = await cache();
    if (!c || typeof c.keys !== 'function') return { deleted: 0 };
    const keep = new Set();
    for (const file of active?.files || []) keep.add(String(file.h).toLowerCase());
    for (const file of previous?.files || []) keep.add(String(file.h).toLowerCase());
    for (const value of Object.values(overrides)) if (value?.h) keep.add(String(value.h).toLowerCase());
    let deleted = 0;
    for (const key of await c.keys()) {
      const hash = hashFromRequest(key);
      if (hash && !keep.has(hash)) { await c.delete(key); deleted++; }
    }
    return { deleted };
  }

  return {
    schema: RESOURCE_CACHE_SCHEMA,
    active,
    get activeManifest() { return active; },
    get previousManifest() { return previous; },
    get pathOverrides() { return overrides; },
    resolveFile,
    remoteUrl,
    url,
    fetchAsset,
    httpUrl: (path) => remoteUrl(resolveFile(path)) || joinUrl(assetBase, path),
    describePath: (path) => `cache:${normalizeAssetPath(path)}`,
    hasPack: () => true,
    refreshManifest,
    setManifestUrl,
    syncOnline,
    commitManifest,
    prefetch,
    installResources,
    gc,
    status: () => ({
      schema: RESOURCE_CACHE_SCHEMA,
      version: active?.version || null,
      previousVersion: previous?.version || null,
      files: active?.files?.length || 0,
      overrides: Object.keys(overrides).length,
      cacheName,
      cacheAvailable: !!cacheApi,
      manifestUrl,
    }),
  };
}
