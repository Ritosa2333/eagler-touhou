/*
 * BRO Web Runtime -- 存档持久化（阶段 1e 前置）
 *
 * 原版 BRO 全部用相对路径写文件：
 *   launch.lua:240      User\setting            设置（JSON，启动期必读）
 *   Lscoredata.lua:22   User\Score.dat          分数/成就/资金（sp:LockString 混淆）
 *   Lscoredata.lua:77   User\Spellcard.dat      符卡记录
 *   Replay.lua:307      User\saving.file        存档槽中间态 → saving1.file
 *   ext_replay.lua:10   replay\slotN.rep        STGR 录像
 * 这些路径在 Emscripten MEMFS 里落在 /User 与 /replay，刷新即丢。
 * 本模块把它们同步到 IndexedDB，启动时在 bro_boot() **之前**灌回 MEMFS：
 * boot.lua 会执行 launch.lua，而 launch.lua 当场就要读 User\setting。
 *
 * 为什么轮询而不是 hook fclose：BRO 的写入点极少（改设置、结算、保存录像），
 * 而且 FileStream 走 io.open，hook 要改 musl/Lua io 库，侵入性远高于
 * "每 interval 帧扫一次这两个小目录"。再补 pagehide/visibilitychange 强制 flush。
 *
 * 时序契约（与 eagler-touhou 的 package/storage 契约一致）：
 *   const s = createStorage(Module);
 *   await s.loadAll();     // 必须在 bro_boot() 之前
 *   ccall('bro_boot');
 *   ...
 *   s.tick(frameNo);       // 每个逻辑帧调用；内部按 interval 节流
 *   await s.flush();       // 页面隐藏/卸载、关卡结算后
 */

const DB_NAME = 'BossRush-save';
const DB_VERSION = 1;
const STORE = 'files';
const LS_KEY = 'BossRush.save.setting';

function openDB(indexedDB) {
  return new Promise((resolve, reject) => {
    if (!indexedDB) { reject(new Error('indexedDB unavailable')); return; }
    let req;
    try { req = indexedDB.open(DB_NAME, DB_VERSION); }
    catch (e) { reject(e); return; }
    req.onupgradeneeded = () => {
      const db = req.result;
      if (!db.objectStoreNames.contains(STORE)) db.createObjectStore(STORE);
    };
    req.onsuccess = () => resolve(req.result);
    req.onerror = () => reject(req.error || new Error('indexedDB open failed'));
    req.onblocked = () => reject(new Error('indexedDB blocked'));
  });
}

function readAll(db) {
  return new Promise((resolve, reject) => {
    const tx = db.transaction(STORE, 'readonly');
    const req = tx.objectStore(STORE).openCursor();
    const out = [];
    req.onsuccess = () => {
      const cur = req.result;
      if (cur) { out.push([cur.key, cur.value]); cur.continue(); }
      else resolve(out);
    };
    req.onerror = () => reject(req.error);
  });
}

function signature(u8) {
  let h = 2166136261 >>> 0;
  for (let i = 0; i < u8.length; i++) {
    h ^= u8[i];
    h = Math.imul(h, 16777619) >>> 0;
  }
  return u8.length + ':' + (h >>> 0).toString(16);
}

function toB64(u8) {
  let s = '';
  const CH = 0x8000;
  for (let i = 0; i < u8.length; i += CH) {
    s += String.fromCharCode.apply(null, u8.subarray(i, i + CH));
  }
  return btoa(s);
}

function fromB64(s) {
  const bin = atob(s);
  const u8 = new Uint8Array(bin.length);
  for (let i = 0; i < bin.length; i++) u8[i] = bin.charCodeAt(i);
  return u8;
}

export function createStorage(Module, opts = {}) {
  const FS = Module.FS;
  const roots = opts.roots || ['/User', '/replay', '/bro/data'];
  const settingPath = opts.settingPath || '/User/setting';
  const interval = opts.interval || 30;
  const idbApi = opts.indexedDB !== undefined ? opts.indexedDB : globalThis.indexedDB;
  const lsApi = opts.localStorage !== undefined ? opts.localStorage : globalThis.localStorage;
  const addEvt = opts.addEventListener !== undefined ? opts.addEventListener : globalThis.addEventListener;

  const known = new Map();       // path -> signature（已落盘的字节指纹）
  const stats = {
    backend: 'memory-only', loaded: 0, saved: 0, skipped: 0,
    syncs: 0, errors: [], lastSync: '',
  };
  let db = null;
  let busy = false;
  let dirty = false;

  const note = (e) => {
    if (stats.errors.length < 12) stats.errors.push(String(e && e.message ? e.message : e));
  };

  const ensureDir = (dir) => {
    if (!dir || dir === '/') return;
    let cur = '';
    for (const p of dir.split('/').filter(Boolean)) {
      cur += '/' + p;
      try { FS.mkdir(cur); } catch { /* 已存在 */ }
    }
  };

  /* 中间态不落盘：saving.file 会在 rename 成 savingN.file 前短暂存在；
     .fake 是原版原子替换残留。.bak 是玩家可见备份，保留。 */
  const transient = (name) => name.endsWith('.fake') || name === 'saving.file';

  const collect = () => {
    const out = [];
    for (const root of roots) {
      const walk = (dir, depth) => {
        if (depth > 4) return;
        let names;
        try { names = FS.readdir(dir); }
        catch { return; }
        for (const n of names) {
          if (n === '.' || n === '..') continue;
          const p = dir === '/' ? '/' + n : dir + '/' + n;
          let st;
          try { st = FS.stat(p); } catch { continue; }
          const isDir = typeof FS.isDir === 'function' ? FS.isDir(st.mode) : !!(st.mode & 0x4000);
          const isFile = typeof FS.isFile === 'function' ? FS.isFile(st.mode) : !!(st.mode & 0x8000);
          if (isDir) { walk(p, depth + 1); continue; }
          if (!isFile || transient(n)) continue;
          try {
            const data = FS.readFile(p);
            const u8 = data instanceof Uint8Array ? data : new Uint8Array(data);
            out.push({ path: p, data: u8, sig: signature(u8) });
          } catch { /* 这一轮读失败就跳过 */ }
        }
      };
      walk(root, 0);
    }
    return out;
  };

  async function loadAll() {
    for (const root of roots) ensureDir(root);

    /* localStorage 镜像优先：IndexedDB 打不开时至少设置不丢。
       但 boot 期 launch.lua 会写回默认 setting，所以镜像必须在 boot 前灌入。 */
    try {
      const mirror = lsApi && lsApi.getItem(LS_KEY);
      if (mirror) {
        const u8 = fromB64(mirror);
        ensureDir(settingPath.slice(0, settingPath.lastIndexOf('/')));
        FS.writeFile(settingPath, u8);
        known.set(settingPath, signature(u8));
        stats.loaded++;
      }
    } catch (e) { note('localStorage mirror: ' + e.message); }

    try { db = await openDB(idbApi); stats.backend = 'indexeddb'; }
    catch (e) {
      stats.backend = lsApi ? 'localStorage-only' : 'memory-only';
      note('openDB: ' + e.message);
      return stats;
    }

    let items;
    try { items = await readAll(db); }
    catch (e) { note('readAll: ' + e.message); return stats; }

    for (const [path, rec] of items) {
      if (typeof path !== 'string') continue;
      if (!roots.some((r) => path.startsWith(r + '/'))) continue;
      const base = path.slice(path.lastIndexOf('/') + 1);
      if (transient(base)) continue;
      let u8;
      try {
        u8 = rec && rec.data instanceof Uint8Array ? rec.data : new Uint8Array((rec && rec.data) || []);
      } catch { continue; }
      try {
        ensureDir(path.slice(0, path.lastIndexOf('/')));
        /* IDB 里的旧文件优先级高于 localStorage 镜像（镜像只是 setting 兜底）。 */
        FS.writeFile(path, u8);
        known.set(path, signature(u8));
        stats.loaded++;
      } catch (e) { note('writeFile ' + path + ': ' + e.message); }
    }
    return stats;
  }

  async function sync(force = false) {
    if (!db) {
      /* 降级：只把 setting 同步写进 localStorage（同步 API，卸载不丢）。 */
      try {
        if (!lsApi) return stats;
        const f = collect().find((x) => x.path === settingPath);
        if (f && known.get(f.path) !== f.sig) {
          lsApi.setItem(LS_KEY, toB64(f.data));
          known.set(f.path, f.sig);
          stats.saved++;
        }
      } catch (e) { note('localStorage sync: ' + e.message); }
      return stats;
    }
    if (busy) { dirty = true; return stats; }
    busy = true;
    try {
      const files = collect();
      const changed = force ? files : files.filter((f) => known.get(f.path) !== f.sig);
      stats.syncs++;
      if (!changed.length) { stats.skipped++; return stats; }

      await new Promise((resolve, reject) => {
        const tx = db.transaction(STORE, 'readwrite');
        const store = tx.objectStore(STORE);
        for (const f of changed) {
          store.put({ data: f.data, t: Date.now() }, f.path);
          if (f.path === settingPath && f.data.length < 65536 && lsApi) {
            try { lsApi.setItem(LS_KEY, toB64(f.data)); } catch { /* 隐私模式 */ }
          }
        }
        tx.oncomplete = () => {
          for (const f of changed) { known.set(f.path, f.sig); stats.saved++; }
          stats.lastSync = new Date().toISOString();
          resolve();
        };
        tx.onerror = () => reject(tx.error || new Error('idb tx failed'));
        tx.onabort = () => reject(tx.error || new Error('idb tx aborted'));
      });
    } catch (e) { note('sync: ' + e.message); }
    finally {
      busy = false;
      if (dirty) { dirty = false; sync(); }
    }
    return stats;
  }

  function tick(frameNo) {
    if (frameNo > 0 && frameNo % interval === 0) sync();
  }
  function flush() { return sync(true); }

  if (addEvt) {
    addEvt('pagehide', () => { flush(); });
    addEvt('visibilitychange', () => {
      try {
        if (globalThis.document && document.visibilityState === 'hidden') flush();
      } catch { /* 无 document（worker）时忽略 */ }
    });
  }

  return {
    loadAll, sync, tick, flush, stats, roots, settingPath,
    inspect: () => collect().map((f) => ({ path: f.path, size: f.data.length, sig: f.sig,
      known: known.get(f.path) || '(new)' })),
    /* 换浏览器 / 备份用：导出成 JSON，或写回 MEMFS 后立刻全量落盘。 */
    exportData: () => ({
      format: 'bro-save-v1',
      exportedAt: new Date().toISOString(),
      files: collect().map((f) => ({ path: f.path, data: toB64(f.data) })),
    }),
    importData: async (obj) => {
      if (!obj || obj.format !== 'bro-save-v1' || !Array.isArray(obj.files)) {
        throw new Error('不是有效的 BRO 存档文件');
      }
      let n = 0;
      for (const f of obj.files) {
        if (!f || typeof f.path !== 'string' || typeof f.data !== 'string') continue;
        if (!roots.some((r) => f.path.startsWith(r + '/'))) continue;  // 防写进只读资源区
        try {
          const u8 = fromB64(f.data);
          ensureDir(f.path.slice(0, f.path.lastIndexOf('/')));
          FS.writeFile(f.path, u8);
          n++;
        } catch (e) { note('import ' + f.path + ': ' + e.message); }
      }
      await sync(true);
      stats.loaded += n;
      return stats;
    },
    wipe: async () => {
      if (db) {
        await new Promise((res) => {
          const tx = db.transaction(STORE, 'readwrite');
          tx.objectStore(STORE).clear();
          tx.oncomplete = res; tx.onerror = res;
        });
      }
      try { if (lsApi) lsApi.removeItem(LS_KEY); } catch { }
      known.clear();
      return stats;
    },
  };
}
