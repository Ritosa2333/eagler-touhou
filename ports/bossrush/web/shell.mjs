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
const resourceCache = createResourceCache({
  assetBase: new URL('./game/', location.href).href,
  manifestUrl: new URL('./dist/package-manifest.json', location.href).href,
});

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
    if (!runtimePromise) {
      const environment = await saveFS.environment();
      runtimePromise = startBRO({ canvas, status, adapter: shell.adapter, resourceCache, params: query, environment })
        .catch((error) => {
          shell.emit({ event: 'error', message: 'boot', error: String(error && error.message || error) });
          if (status) status.textContent = `ERROR: ${error && error.message || error}`;
          throw error;
        });
    }
    return runtimePromise;
  },
});

void prepareData();
globalThis.__broShell = shell;
