/* BRO eagler runtime 入口壳：初始化增量资源缓存 → 安装协议壳 → 等 launch 启动游戏。 */

// 兼容 BRO 源树（src/）与 eagler 扁平 runtime 目录。
const { createResourceCache } = await import('./src/resource_cache.mjs')
  .catch(() => import('./resource_cache.mjs'));
const { startBRO, createBROEnvironment } = await import('./src/runtime_entry.mjs')
  .catch(() => import('./runtime_entry.mjs'));
const { createLazySaveFS } = await import('./src/save_files.mjs')
  .catch(() => import('./save_files.mjs'));
const { installManagedShell, prepareManagedData } = await import('./managed-shell.mjs');

const canvas = document.getElementById('screen');
const status = document.getElementById('status');
if (!canvas) throw new Error('BRO runtime canvas #screen is missing');

const query = new URLSearchParams(location.search);
// 线上增量清单地址，优先级：URL ?manifestUrl= > <meta name="eagler-bro-manifest"> > 默认本地清单。
// 导入包形态下本地清单可能不存在，syncOnline 会返回 offline 并静默降级，不阻塞启动。
const metaManifest = typeof document !== 'undefined'
  ? document.querySelector?.('meta[name="eagler-bro-manifest"]')?.getAttribute('content') : null;
const manifestUrl = query.get('manifestUrl') || metaManifest
  || new URL('./dist/package-manifest.json', location.href).href;
const assetBase = query.get('assetBase') || new URL('./game/', location.href).href;
const resourceCache = createResourceCache({ assetBase, manifestUrl });

// 存档桥：file session（launcher 的存档下载/上传）发生在 launch 之前，
// 此时 Lua 尚未 boot；直接用 Module.FS + storage.flush 才能读写真存档。
const note = (message) => {
  if (status) status.textContent = `${status.textContent || ''}\n${message}`.trim();
};
const saveFS = createLazySaveFS({
  createEnvironment: () => createBROEnvironment({ note }),
});

let runtimePromise = null;
let dataPromise = null;
let shell = null;
// #35 加载优化：首屏需要的组件组先预热，music/se 这类大块交给按需路径
// （BGM 走流式分片、SE 由 audio.warm 在启动后自己补），否则首次要下 191MB。
const PREFETCH_GROUPS = ['images-ui', 'fonts', 'shader', 'misc', 'scripts', 'images-bg'];
let prefetchPromise = null;
const prefetchInBackground = () => {
  if (prefetchPromise || !resourceCache?.prefetchManifest) return prefetchPromise;
  let last = 0;
  prefetchPromise = resourceCache.prefetchManifest({
    groups: PREFETCH_GROUPS,
    concurrency: 3,
    onProgress: (p) => {
      // 进度事件限流：每 10 个文件或每年 800ms 报一次，别把启动器事件通道刷满。
      const now = Date.now();
      if (p.downloaded - last < 10 && now - (prefetchPromise.lastAt || 0) < 800) return;
      last = p.downloaded;
      prefetchPromise.lastAt = now;
      shell.emit({ event: 'transfer', phase: 'prefetch', ...p });
    },
    onError: (error, file) => shell.emit({
      event: 'error', message: 'prefetch', path: file?.p || '', error: String(error && error.message || error),
    }),
  }).catch(() => null);
  return prefetchPromise;
};
const waitFirstFrame = async (runtime) => {
  // 首帧判定：rAF 已经跑过一帧（runtime.frames 由 runtime_entry 累加）。
  // 最多等 30 秒，超时也照样开始预热，避免异常路径下永不预热。
  const deadline = Date.now() + 30000;
  while (Date.now() < deadline) {
    if (Number(runtime?.frames) > 0) return true;
    await new Promise((r) => setTimeout(r, 100));
  }
  return false;
};
const prepareData = () => {
  if (!dataPromise) {
    dataPromise = prepareManagedData({ game: 'bro', query, epoch: shell?.navigation?.epoch })
      .catch((error) => {
        shell?.emit?.({ event: 'error', message: 'managed-data', error: String(error && error.message || error) });
        return null;
      });
  }
  return dataPromise;
};

shell = installManagedShell({
  game: 'bro',
  resourceCache,
  saveFS,
    start: async () => {
      await prepareData();
      // 线上增量资源：启动路径只拉清单并提交（commitOnly），绝不等整包下完。
      // 提交后按需 fetchAsset 就会走 HTTP + 逐文件入库；其余由 prefetchInBackground
      // 在首帧之后按组件组慢慢填缓存。offline 时静默降级，不阻塞启动。
      if (resourceCache?.syncOnline) {
        try {
          const sync = await resourceCache.syncOnline({ commitOnly: true });
          shell.emit({ event: 'transfer', phase: 'manifest', ...sync });
          note(`resources: ${sync.status}${sync.downloads ? ` +${sync.downloads} pending` : ''}`);
        } catch (error) {
          shell.emit({ event: 'error', message: 'manifest', error: String(error && error.message || error) });
        }
      }
      if (!runtimePromise) {
        const environment = await saveFS.environment();
      runtimePromise = startBRO({ canvas, status, adapter: shell.adapter, resourceCache, params: query, environment })
        .catch((error) => {
          shell.emit({ event: 'error', message: 'boot', error: String(error && error.message || error) });
          if (status) status.textContent = `ERROR: ${error && error.message || error}`;
          throw error;
        });
        // 首帧之后再预热：首帧前抢带宽会拖慢 wasm/字体，反而更慢出画面。
        void runtimePromise.then((runtime) => waitFirstFrame(runtime).then(() => prefetchInBackground()));
    }
    return runtimePromise;
  },
});

void prepareData();
globalThis.__broShell = shell;
