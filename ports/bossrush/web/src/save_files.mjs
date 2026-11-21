/*
 * BRO eagler 存档文件桥（阶段 2）
 *
 * eagler 的 storage.scoreFile 是相对名 'Score.dat'，saveRoot 声明为 '/bro'；
 * BRO 本体却把存档写在 /User/Score.dat、把录像写在 /replay/。直接把宿主
 * 路径透传给 MEMFS 会读写到 /Score.dat（空文件），这是阶段 2 的真缺口。
 *
 * 本模块做两件事：
 *   1. eagler 相对路径 ↔ BRO MEMFS 路径的双向映射；
 *   2. 单文件白名单 + 只写数据文件，挡住 RGO 式整目录覆盖（/User/setting
 *      是网页权威源、/bro/data 是 Lua 补丁表，永远不接受宿主写入）。
 *
 * file session 发生在 launch 之前，Lua 的 BRO_fs_* 还没有 boot。因此这里
 * 直接用 Emscripten FS（Module.FS），并把写后 flush 到 IndexedDB，保证
 * launcher 的 restart({sync:false}) 后新实例 loadAll 能读到刚导入的字节。
 */

const SAVE_FILES = new Set([
  'Score.dat', 'Score.dat.bak',
  'Spellcard.dat', 'Spellcard.dat.bak',
]);
const SAVE_NUMBERED = /^saving\d+\.file$/i;
const REPLAY_FILE = /^replay\/([A-Za-z0-9][A-Za-z0-9._-]*\.(?:rep|multi))$/i;
const BASE_NAME = /^[^/\\]+$/;

/** eagler 视图路径 → BRO MEMFS 绝对路径；不在白名单内一律拒绝。 */
export function resolveSavePath(value, opts = {}) {
  const write = !!opts.write;
  let s = String(value == null ? '' : value).replace(/\\/g, '/').replace(/^\/+/, '');
  if (s.toLowerCase().startsWith('bro/')) s = s.slice(4);
  if (s.toLowerCase().startsWith('user/')) s = s.slice(5);
  // 段级穿越检查必须在取 basename 之前，否则 replay/../../Score.dat
  // 会被归一化成合法存档名而绕过白名单意图。
  if (s.split('/').some((seg) => seg === '..' || seg === '.')) {
    throw new Error('BRO save path rejected: ' + String(value));
  }
  const replay = REPLAY_FILE.exec(s);
  if (replay) return '/replay/' + replay[1];
  const base = s.slice(s.lastIndexOf('/') + 1);
  if (!BASE_NAME.test(base) || base === '.' || base === '..') {
    throw new Error('BRO save path rejected: ' + String(value));
  }
  if (base.toLowerCase() === 'setting') {
    if (write) throw new Error('BRO setting is web-authoritative; upload rejected');
    return '/User/setting';
  }
  if (SAVE_FILES.has(base) || SAVE_NUMBERED.test(base)) return '/User/' + base;
  throw new Error('BRO save path rejected: ' + String(value));
}

/** BRO MEMFS 绝对路径 → eagler 视图路径；内部文件返回 null（不暴露给网页）。 */
export function publicSavePath(value) {
  const s = String(value == null ? '' : value).replace(/\\/g, '/');
  if (s.startsWith('/User/')) return s.slice('/User/'.length);
  if (s.startsWith('/replay/')) return 'replay/' + s.slice('/replay/'.length);
  return null;
}

/** 延迟初始化：file session 在 launch 前发生，Module 此时可能尚未创建。 */
export function createLazySaveFS(options = {}) {
  let envPromise = null;
  const environment = () => {
    if (!envPromise) {
      envPromise = Promise.resolve().then(() => {
        if (typeof options.createEnvironment !== 'function') {
          throw new Error('save file bridge has no environment factory');
        }
        return options.createEnvironment();
      });
    }
    return envPromise;
  };
  const use = async (fn) => fn(await environment());
  const toBytes = (value) => {
    if (value instanceof Uint8Array) return Array.from(value);
    if (Array.isArray(value)) return value.map((n) => Math.max(0, Math.min(255, Number(n) | 0)));
    if (value && typeof value.length === 'number') return Array.from(new Uint8Array(value));
    return [];
  };
  return {
    environment,
    ready: async () => { await environment(); return true; },
    list: () => use(({ storage }) => {
      const items = typeof storage?.inspect === 'function' ? storage.inspect() : [];
      return (Array.isArray(items) ? items : []).map((item) => {
        const path = item && typeof item.path === 'string' ? item.path : '';
        const visible = publicSavePath(path);
        if (!visible) return null;
        const size = item && Number.isFinite(Number(item.size)) ? Math.max(0, Math.round(Number(item.size))) : 0;
        return { path: visible, size };
      }).filter(Boolean);
    }),
    read: (path) => {
      const fsPath = resolveSavePath(path);
      return use(({ Module }) => {
        const data = Module.FS.readFile(fsPath);
        return toBytes(data);
      });
    },
    write: (path, bytes) => {
      const fsPath = resolveSavePath(path, { write: true });
      return use(async ({ Module, storage }) => {
        const dir = fsPath.slice(0, fsPath.lastIndexOf('/'));
        try { Module.FS.stat(dir); } catch { Module.FS.mkdir(dir); }
        const data = new Uint8Array(toBytes(bytes));
        Module.FS.writeFile(fsPath, data);
        // restart({sync:false}) 会重建实例；新实例 loadAll 从 IndexedDB 灌回，
        // 所以这里必须先把刚写入的字节落盘，否则校验读到的是旧档。
        if (storage && typeof storage.flush === 'function') await storage.flush();
        return true;
      });
    },
    remove: (path) => {
      const fsPath = resolveSavePath(path, { write: true });
      return use(async ({ Module, storage }) => {
        if (storage && typeof storage.removeFile === 'function') {
          await storage.removeFile(fsPath);
          return true;
        }
        try { Module.FS.unlink(fsPath); } catch { /* 不存在也算成功 */ }
        if (storage && typeof storage.flush === 'function') await storage.flush();
        return true;
      });
    },
  };
}
