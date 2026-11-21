/*
 * BRO managed runtime shell -- eagler-touhou/1 协议壳
 *
 * 这里只做协议边界：epoch/origin/source 门禁、ready/transfer/error 事件、
 * configure/resources/launch 命令排队。真正的游戏启动交给 runtime_entry.mjs。
 *
 * 注意：字符串 runtimeEpoch / __eaglerPrepareManagedRuntimeDataV1 是 eagler
 * 的契约标记，工具链会扫描源码校验，不要改名。
 */

// 同时兼容 BRO 源树（src/ 子目录）与 eagler 部署后的目录型 runtime（扁平化）。
const { EaglerRuntimeAdapter } = await import('./src/eagler_adapter.mjs')
  .catch(() => import('./eagler_adapter.mjs'));

export const RUNTIME_EPOCH_QUERY_PARAMETER = 'runtimeEpoch';
export const HOST_PROTOCOL = 'eagler-touhou/1';

export function readRuntimeEpoch(search = '') {
  const raw = new URLSearchParams(search).get(RUNTIME_EPOCH_QUERY_PARAMETER);
  const value = Number(raw);
  return Number.isSafeInteger(value) && value > 0 ? value : null;
}

/** 读取 Launcher 注入的 managed DATA（BRO 不写 /bro.data，只校验身份后继续）。 */
export async function prepareManagedData(options = {}) {
  const parentWindow = options.parentWindow || (typeof parent !== 'undefined' ? parent : null);
  const query = options.query || new URLSearchParams(
    typeof location !== 'undefined' ? location.search : '',
  );
  if (query.get('managedData') !== '1') return null;
  const provider = parentWindow?.__eaglerPrepareManagedRuntimeDataV1;
  if (typeof provider !== 'function') return null;
  const epoch = options.epoch;
  const game = options.game || 'bro';
  const request = { game, generation: query.get('gameGeneration'), epoch };
  const prepared = await provider(request);
  return { game, epoch, fileId: prepared?.fileId || null, bytes: prepared?.bytes || prepared?.buffer?.byteLength || 0 };
}

export function installManagedShell(options = {}) {
  const game = String(options.game || 'bro');
  const locationObj = options.location || (typeof location !== 'undefined' ? location : { href: 'https://bro.invalid/', origin: 'https://bro.invalid', search: '' });
  const parentWindow = options.parentWindow !== undefined
    ? options.parentWindow
    : (typeof parent !== 'undefined' ? parent : null);
  const query = new URLSearchParams(locationObj.search || '');
  const standalone = query.get('standalone') === '1';
  const inFrame = !!parentWindow && parentWindow !== globalThis;
  if (!inFrame && !standalone) throw new Error('Launch BRO Runtime through eagler-touhou');

  const epoch = readRuntimeEpoch(locationObj.search || '') ?? (standalone ? 1 : null);
  if (!Number.isSafeInteger(epoch) || epoch <= 0) throw new Error('Invalid runtimeEpoch');
  const navigation = { epoch, origin: locationObj.origin };
  const target = inFrame ? parentWindow : globalThis;
  const emit = (payload) => {
    const message = { protocol: HOST_PROTOCOL, game, epoch, ...payload };
    target.postMessage({ protocol: HOST_PROTOCOL, game, epoch, ...payload }, navigation.origin);
    return message;
  };

  const adapter = new EaglerRuntimeAdapter({
    game,
    epoch,
    post: emit,
    resourceCache: options.resourceCache || null,
    launchHook: options.start || null,
    messageTarget: inFrame ? parentWindow : null,
    allowedOrigin: navigation.origin,
    saveRoot: '/bro',
    // 阶段2：file session 在 launch 之前读写存档，必须走 JS FS 桥，
    // 不能等 Lua 的 BRO_fs_*（bro_boot 之后才存在）。
    fs: options.saveFS || null,
    periodicMs: 1000,
  });

  let queue = Promise.resolve();
  let announced = false;
  const announceReady = (extra = {}) => {
    if (announced) return;
    announced = true;
    emit({ event: 'ready', saveRoot: adapter.saveRoot, ...extra });
  };

  const onMessage = (event) => {
    if (inFrame && event.source !== parentWindow) return;
    if (event.origin && event.origin !== navigation.origin) return;
    const message = event.data;
    if (!message || message.protocol !== HOST_PROTOCOL || message.game !== game) return;
    if (message.epoch !== navigation.epoch) return;
    if (typeof message.command !== 'string') return;
    queue = queue.then(async () => {
      try {
        if (message.command === 'launch' && !announced) {
          announceReady({ prelaunch: true });
        }
        await adapter._handleMessage(message);
      } catch (error) {
        emit({ event: 'error', message: `command:${message.command}`, error: String(error && error.message || error) });
        if (typeof message.request === 'string') {
          emit({ request: message.request, ok: false, error: String(error && error.message || error) });
        }
      }
    });
  };
  if (typeof addEventListener === 'function') addEventListener('message', onMessage);

  const shell = {
    adapter,
    navigation,
    emit,
    announceReady,
    dispose() {
      if (typeof removeEventListener === 'function') removeEventListener('message', onMessage);
      adapter.detach();
    },
  };
  announceReady({ resourceCache: options.resourceCache?.status?.() || null });
  return shell;
}
