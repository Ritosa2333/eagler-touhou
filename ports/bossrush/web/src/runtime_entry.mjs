/*
 * BRO Web Runtime -- 正式入口的启动/主循环
 *
 * 把原先 play.html / persist.html / eagler.html 三处分散的接线合流：
 *   resource cache（可选） → createBROModule → storage.loadAll（boot 前）
 *   → bro_boot → renderer/audio/input → 60Hz 固定逻辑帧 + rAF 呈现
 *   → storage.tick（每 30 帧）/ flush（pagehide / visibilitychange）
 *
 * 资源加载保持 BRO 的既有语义：png/ogg 走 JS HTTP，不进 MEMFS。
 * resource_cache 只是给 assets.* 加上内容寻址缓存与增量更新能力。
 */

import { createStorage } from './storage.mjs';
import { createRenderer } from './renderer.mjs';
import { attachInput } from './input.mjs';
import { createAudioLayer } from './audio.mjs';

const FONT_REGISTRY = [
  ['title', 'THlib/UI/font/text.ttf', 32],
  ['boss_name', 'THlib/UI/font/text.ttf', 20],
  ['big_text', 'THlib/UI/font/text.ttf', 80],
  ['sc_menu', 'THlib/UI/font/text.ttf', 26],
  ['achievement', 'THlib/UI/font/text.ttf', 28],
  ['manual', 'THlib/UI/font/text.ttf', 25],
  ['pretty', 'THlib/UI/font/text2.ttf', 32],
];

function httpAssets(assetBase) {
  const enc = (p) => String(p).replace(/\\/g, '/').replace(/^\/+/, '')
    .split('/').map(encodeURIComponent).join('/');
  const url = (p) => `${assetBase}${enc(p)}`;
  return {
    url: async (p) => url(p),
    httpUrl: url,
    describePath: (p) => `http:${String(p)}`,
    hasPack: () => false,
    fetchAsset: (p) => fetch(url(p)),
  };
}

// BadApple 逐帧光点数据（file1..7 + lyrics）不再 embed 进 bro.wasm，改为和
// 其他游戏素材一样走 HTTP/资源站分发。web 端只分发 BA1 编码的 fileN.ba1
// （21.7MB → 5.18MB，4.2x），boot 前在此解码回明文写入 MEMFS /BadApple/，
// Lua 侧 io.lines("BadApple\\fileN") 经 prelude 转正斜杠后读到的是原文本，
// 因此 game/mod/BadApple.lua 与原生读取路径都无需改动。
// 缺少某个文件时静默跳过：prelude 的 io.lines 空壳迭代器会让该章节无画面，
// 但不会阻断 boot / 其他章节（与内嵌版缺失时的降级行为一致）。
const BAD_APPLE_FILES = [
  'file1', 'file2', 'file3', 'file4', 'file5', 'file6', 'file7', 'lyrics',
];

// BA1（小端）："BA1\n"(4B) + u32 帧数；每帧 u16 变化点数，随后
// [u8 gap][u8 value] 重复；gap = 与上一变化点的下标间距-1（首点相对 -1），
// gap>=255 时写 0xFF + u16 gap。原行只列相对上一帧变化的格子，按 idx 升序。
// 还原规则：`S` + 每个变化点 `[A x y]`(value=255) / `[v x y]`，行尾补 \n。
function decodeBA1(bytes) {
  if (bytes.length < 8 || bytes[0] !== 0x42 || bytes[1] !== 0x41 ||
      bytes[2] !== 0x31 || bytes[3] !== 0x0a) {
    throw new Error('not BA1 data');
  }
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const frames = view.getUint32(4, true);
  let p = 8;
  const lines = new Array(frames);
  for (let f = 0; f < frames; f++) {
    const count = view.getUint16(p, true);
    p += 2;
    let line = 'S';
    let pos = -1;
    for (let i = 0; i < count; i++) {
      let gap = bytes[p++];
      if (gap === 255) {
        gap = view.getUint16(p, true);
        p += 2;
      }
      const value = bytes[p++];
      pos += gap + 1;
      const x = pos % 32;
      const y = (pos / 32) | 0;
      line += value === 255 ? `[A ${x} ${y}]` : `[${value} ${x} ${y}]`;
    }
    lines[f] = line;
  }
  if (p !== bytes.length) throw new Error(`BA1 trailing bytes: ${p} != ${bytes.length}`);
  return new TextEncoder().encode(lines.join('\n') + '\n');
}

async function loadBadAppleIntoMEMFS(Module, assets, note) {
  const FS = Module.FS;
  if (!FS) return;
  try { FS.mkdir('/BadApple'); } catch (e) { /* 已存在 */ }
  // 8 个文件并行拉取，避免串行下载拖慢首启；fileN.ba1 在写入前本地解码。
  await Promise.all(BAD_APPLE_FILES.map(async (name) => {
    try {
      const remote = name === 'lyrics' ? name : `${name}.ba1`;
      const res = await assets.fetchAsset(`BadApple/${remote}`);
      if (!res || !res.ok) throw new Error(`HTTP ${res && res.status}`);
      let bytes = new Uint8Array(await res.arrayBuffer());
      if (name !== 'lyrics') bytes = decodeBA1(bytes);
      FS.writeFile(`/BadApple/${name}`, bytes);
    } catch (e) {
      note(`BadApple/${name} load failed (chapter may be empty): ${e.message}`);
    }
  }));
}

/** 创建 Module 与 storage 但不 boot。eagler 的 file session 在 launch 之前
 *  就要读写存档，因此这段必须与 startBRO 解耦；loadAll 仍在 boot 之前。 */
export async function createBROEnvironment(options = {}) {
  const note = typeof options.note === 'function' ? options.note : () => {};
  note('loading wasm...');
  // BRO 源树中 glue 在 ../build/bro.mjs；eagler 部署后与 runtime 模块同目录。
  // 用成功 import 的 URL 推导 wasm 基址，不能用 HTML 目录（源树下 wasm 在 build/）。
  let createBROModule = null;
  let detectedModuleBase = null;
  for (const candidate of ['./bro.mjs', '../build/bro.mjs', '../bro.mjs']) {
    try {
      const mod = await import(candidate);
      if (mod?.default) {
        createBROModule = mod.default;
        detectedModuleBase = new URL('.', new URL(candidate, import.meta.url)).href;
        break;
      }
    } catch { /* 尝试下一个布局 */ }
  }
  if (!createBROModule) throw new Error('bro.mjs glue not found');
  const moduleBase = options.moduleBase || detectedModuleBase;
  const Module = await createBROModule({ locateFile: (p) => new URL(p, moduleBase).href });
  const ccall = Module.ccall;
  const ev = (code) => ccall('bro_eval', 'string', ['string'], [code]);
  globalThis.__broModule = Module;
  globalThis.__broEval = ev;

  note('loading saves...');
  const storage = createStorage(Module);
  // B2 铁律：launch.lua 在 bro_boot() 当场读 User/setting，loadAll 必须先行。
  const loadStats = await storage.loadAll();
  return { Module, ccall, ev, storage, loadStats, moduleBase };
}

export async function startBRO(options = {}) {
  const canvas = options.canvas;
  if (!canvas) throw new Error('BRO runtime requires a canvas');
  const adapter = options.adapter || null;
  const resourceCache = options.resourceCache || null;
  const assetBase = String(options.assetBase || '/game/').replace(/\/?$/, '/');
  const assets = resourceCache || httpAssets(assetBase);
  const params = options.params || new URLSearchParams(
    typeof location !== 'undefined' ? location.search : '',
  );
  const status = options.status || null;
  const note = (message) => {
    if (status) status.textContent = `${status.textContent || ''}\n${message}`.trim();
  };

  const env = options.environment || await createBROEnvironment({ moduleBase: options.moduleBase, note });
  const { Module, ccall, ev, storage, loadStats } = env;
  if (adapter) {
    adapter.ccall = ccall;
    adapter.Module = Module;
    adapter.storage = storage;
  }

  // BadApple 数据从资源站/HTTP 拉取并写入 MEMFS，必须在 bro_boot（Lua 加载
  // BadApple.lua 时会 io.lines("BadApple\\lyrics")）之前完成。
  note('loading badapple data...');
  await loadBadAppleIntoMEMFS(Module, assets, note);

  note('booting lua...');
  const boot = ccall('bro_boot', 'number', [], []);
  if (boot !== 0) throw new Error(`bro_boot failed: ${boot} ${ccall('bro_stats', 'string', [], [])}`);
  // configure/launch 阶段 ccall 尚未注入、Lua VM 尚未建立，两次 apply 都是 no-op。
  // 这里是唯一真正的下发点：必须在 complete_load（BRO_start_normal/BRO_enter_stage）
  // 之前完成，legacy 模式与 chapter 首关才能生效。
  if (adapter && typeof adapter.applyAfterBoot === 'function') adapter.applyAfterBoot();

  // 观战阶段 A：configure 时若带 netplaySpectator / player=0+观战发布标志，
  // 这里（Lua VM 就绪后）才真正开 socket：观战端连 ?spectator=，发布端连 data socket。
  // 普通单机 / 普通联机两个 setup 都会因缺标志直接 no-op。
  if (adapter && typeof adapter.setupSpectator === 'function') {
    try { adapter.setupSpectator(); } catch (e) { console.warn('setupSpectator failed', e); }
    try { adapter.setupSpectatorPublisher(); } catch (e) { console.warn('setupSpectatorPublisher failed', e); }
  }

  const renderer = createRenderer(Module, canvas, {
    assetBase,
    assets,
    preserveDraw: true,
  });
  globalThis.__broRenderer = renderer;
  const input = attachInput({ ccall, canvas });
  if (adapter) adapter.input = input;
  globalThis.__broInput = input;

  const audio = createAudioLayer(Module, {
    assetBase,
    assets,
    force: params.get('audio') === 'force',
  });
  globalThis.__broAudio = audio;

  // scene 贴图注册发生在 bro_boot 之后；必须先刷资源表再请求全部纹理。
  renderer.loadResourceTable();
  // #35 加载优化：首帧不再等全部贴图（线上 536 张要几十秒，启动器会超时）。
  // 只给 800ms 预算，剩下的仍在 pending 队列里继续取，bindTex 画到时补上。
  await renderer.requestAll({ budgetMs: 800 });
  for (const [name, path, size] of FONT_REGISTRY) {
    renderer.text.registerFont(name, { kind: 8, path, size });
  }
  await renderer.text.prepare();
  audio.flush();
  audio.syncResources();
  // #35：SE 预解码（24MB/178 个文件）不再挡在首帧前，改成后台跑；
  // 首帧之后才开始，点菜单第一声若还没预热完最多晚一两帧。
  if (params.get('audioNoWarm') !== '1') {
    setTimeout(() => {
      Promise.resolve(audio.warm({ concurrency: 4 }))
        .catch((error) => console.warn('audio warm failed', error));
    }, 1200);
  }

  const resParam = params.get('res');
  if (resParam) {
    const match = /^(\d+)[xX](\d+)$/.exec(resParam);
    if (match) ev(`return tostring(BRO_web_apply_resolution(${+match[1]},${+match[2]}))`);
  }
  const sz = ev('return tostring(setting.resx) .. "," .. tostring(setting.resy)').split(',').map(Number);
  ccall('bro_set_screen_size', null, ['number', 'number'], [sz[0] || 1280, sz[1] || 720]);

  const directStage = params.get('stage');
  if (directStage) {
    const entered = ev(`return tostring(BRO_enter_stage(${JSON.stringify(directStage)}))`);
    if (entered !== 'true') throw new Error(`enter stage failed: ${entered}`);
  } else {
    const started = ev("return tostring(BRO_start_normal('TIDEW'))");
    if (started !== 'true') throw new Error(`start_normal failed: ${started}`);
    for (let i = 0; i < 90; i++) ccall('bro_frame', 'number', [], []);
  }
  ev('lstg.var.ran_seed = 12345; return true');
  ev('lstg.tmpvar.hiscore = 0; return true');

  // 阶段 3AZ：正式房间流程 → 联机主循环。
  // 以前这里只走单机推进（syncHeld + bro_frame），门面从未建链、也从没人给
  // 远端生成真身；房间里的对手只是个 POS 代理。现在若 configure 带
  // netplayMode/netplayUrl，就在进关之后（Lua 表与 init_player_data 即将就绪）
  // 建链、下发房间人数与各槽自机、发起 match，主循环改走门面 stepGame。
  if (adapter && typeof adapter.startNetplayIfConfigured === 'function') {
    try {
      const net = await adapter.startNetplayIfConfigured();
      if (net && net.wired) {
        note(`netplay wired: room=${net.room} slot=${net.player}/${net.playerCount} slots=${net.slots}`);
      }
    } catch (error) {
      console.warn('netplay wiring failed', error);
      note('netplay wiring failed: ' + String(error && error.message || error));
    }
  }

  renderer.loadResourceTable();
  // #35：进关后的第二轮预热同样带预算（进关后本来就有淡入，晚一两帧无所谓）。
  await renderer.requestAll({ budgetMs: 600 });

  const runtime = {
    state: 'running',
    frames: 0,
    fps: 0,
    lock60: true,
    module: Module,
    storage,
    renderer,
    input,
    audio,
    assets,
    loadStats,
    getFrameHealth: () => ({
      fps: runtime.fps,
      maxGapMs: runtime.maxGapMs || 0,
      frameMs: runtime.frameMs || 16.7,
    }),
    getAudioHealth: () => ({
      queuedMs: audio.stats?.queuedMs || 0,
      backend: audio.backend || 'webaudio',
      underruns: audio.stats?.underruns || 0,
    }),
    flush: () => storage.flush(),
  };
  globalThis.__broRuntime = runtime;
  note('running');

  let last = performance.now();
  let accumulator = 0;
  let fpsSince = last;
  let fpsFrames = 0;
  let firstFrameSent = false;
  let firstFrameSerial = null;
  let lastCanvasW = canvas.width;
  let lastCanvasH = canvas.height;
  let stopped = false;
  const stepMs = 1000 / 60;

  /* #39 线上白屏的真因（改这里之前先读完）：
   *
   * 原生版 `stage.QuitGame()`（game/THlib/lib/Lstage.lua:48）只置
   * `lstg.quit_flag = true`，靠进程自己退出收场。网页运行时没有「进程」可退，
   * 以前（web/src/web_patches.lua:442-456 置 `__bro_reload_requested`，本文件
   * 下面的轮询）一律用 `location.reload()` 当退出实现 —— 单页 standalone 下
   * 这没问题，但在**线上 eagler 启动器形态**下必白屏：
   *   1) BRO 是 launcher 用 `runtime.location.replace(url)` 载进 iframe 的子文档，
   *      `document` 的身份被登记在 launcher/app/services/runtime.ts 的
   *      `runtimeDocument` 里（:565-575 每次 onMessage 都比对）；
   *   2) 子页 `location.reload()` 会换掉 document → `runtimeIdentity()`
   *      （:479-483 的 `runtime.document !== runtimeDocument`）判定
   *      「Runtime document was replaced」→ `reset('error')`，而 reset 会把 iframe
   *      导航到 `about:blank`（:441-449）——这就是玩家看到的纯白屏；
   *   3) 就算绕过身份比对也没救：`ready` 事件只认一次
   *      （:592 `if (message.event === 'ready' && !snapshot.ready)`），重载后的
   *      新 ready 直接被丢掉，永远等不到第二次 configure/launch。
   *
   * 协议里本来就留了正经出口：`event === 'exit'`（launcher runtime.ts:646-651），
   * 按 `status`/`code` 干净地 `reset('exited')` 回启动器（默认值在
   * eagler_adapter.mjs:74 事件白名单里早就有 'exit'，只是 BRO 从来没发过）。
   * 所以：iframe 内 → 先 flush 存档，再发 exit；只有真·单页（无 parent）才
   * 继续整页重载。存档必须先落盘，MEMFS 会随文档一起销毁。
   */
  const framed = typeof window !== 'undefined' && !!window.parent && window.parent !== window;
  let quitting = false;
  function requestQuit() {
    if (quitting) return;
    quitting = true;
    stopped = true;                 // 先停逻辑/渲染循环：QuitGame 之后不等同于「继续跑」
    runtime.state = 'exited';
    try { audio.stopAll?.(); } catch { /* 音频层可能已停 */ }
    const finish = () => {
      if (framed && adapter && typeof adapter._emit === 'function') {
        try { adapter._emit('exit', { status: 'success', code: 0 }); } catch { /* 宿主已关 */ }
        return;
      }
      try { location.reload(); } catch { /* ignore */ }
    };
    let done = false;
    const once = () => { if (done) return; done = true; finish(); };
    try { Promise.resolve(storage.flush()).then(once, once); } catch { once(); }
    // 兜底：落盘卡住也必须退出，不能让玩家卡在停止的画面上。
    try { setTimeout(once, 3000); } catch { /* ignore */ }
  }

  function frame(now) {
    if (stopped) return;
    try {
      let dt = now - last;
      last = now;
      if (dt > 100) dt = 100;
      accumulator += dt;
      let stepped = 0;
      // 观战（阶段 A/B）：观战端不采本地键盘、不自走 bro_frame——
      // 一切由 SpectatorClient.pump() 按 relay 送来的确认输入逐帧驱动；
      // 无帧 / 暂停 / 定位缺帧时停表（accumulator 归零），避免暂停攒出一大堆
      // 待补的时间步长在恢复后一次性灌进逻辑。
      const spectator = adapter && adapter.spectatorClient ? adapter.spectatorClient : null;
      // 联机主循环（阶段 3AZ）：输入交换 / 预测修正 / POS / DIGEST 全在门面
      // stepGame 里，这里只负责「本帧要不要推进」和计数。栅栏未开帧时不扣时间步长。
      const netplay = (!spectator && adapter && adapter.multi && adapter.multiStarted)
        ? adapter.multi : null;
      if (spectator) {
        while (accumulator >= stepMs && stepped < 4) {
          const r = adapter.pumpSpectatorFrame();
          if (!r || !r.advanced) { accumulator = 0; break; }
          storage.tick(++runtime.frames);
          accumulator -= stepMs;
          stepped++;
        }
      } else if (netplay) {
        while (accumulator >= stepMs && stepped < 4) {
          input.syncHeld();
          const r = adapter.stepNetplayFrame(runtime.frames + 1);
          if (!r || !r.advanced) { accumulator = 0; break; }
          if (adapter.publishSpectatorFrame && adapter.spectatorUploader) {
            try {
              const rows = adapter._luaJsonCall('BRO_spectator_rows') || [];
              const seed = adapter.netplay?.seed || 0;
              adapter.publishSpectatorFrame(runtime.frames, seed, rows);
            } catch { /* 观战尽力而为 */ }
          }
          storage.tick(++runtime.frames);
          accumulator -= stepMs;
          stepped++;
        }
      } else {
        while (accumulator >= stepMs && stepped < 4) {
          input.syncHeld();
          ccall('bro_frame', 'number', [], []);
          // 发布端：用本机/全玩家当前输入组装 0xe8 观战帧上传（失败仅丢帧，不影响对局）。
          if (adapter && typeof adapter.publishSpectatorFrame === 'function'
              && adapter.spectatorUploader) {
            try {
              const rows = adapter._luaJsonCall('BRO_spectator_rows') || [];
              const seed = adapter.netplay?.seed || 0;
              adapter.publishSpectatorFrame(runtime.frames, seed, rows);
            } catch { /* 观战尽力而为 */ }
          }
          storage.tick(++runtime.frames);
          accumulator -= stepMs;
          stepped++;
        }
      }
      if (!runtime.lock60 || stepped > 0) {
        ccall('bro_render', 'number', [], []);
        renderer.drawFrame();
      } else {
        renderer.presentCached();
      }
      if (canvas.width !== lastCanvasW || canvas.height !== lastCanvasH) {
        lastCanvasW = canvas.width; lastCanvasH = canvas.height;
        input.syncCanvas();
      }
      audio.consume();

      // #37：每个 launchSerial 都要发一次 first-frame —— 房间现在复用同一个
      // WASM 实例连开第二局（预览 → 开始游戏 → 回主菜单 → 再开始游戏），
      // 而 eagler 的 launch() 只认当前 serial 的 first-frame（runtime.ts:590
      // 会丢弃 launchSerial 不符的回包）。一次性布尔会让第二局一直等到超时。
      if (!firstFrameSent || firstFrameSerial !== adapter?.launchSerial) {
        firstFrameSent = true;
        firstFrameSerial = adapter?.launchSerial ?? null;
        adapter?.notifyFirstFrame?.(adapter.launchSerial);
      }
      fpsFrames++;
      if (now - fpsSince >= 500) {
        // #39：QuitGame（web_patches.lua:442-456）置 __bro_quit_restart + __bro_reload_requested，
        // 必须走 requestQuit（iframe 内发协议 exit，单页才重载）——重载在启动器里
        // 会被判成「document 被替换」并白屏，详见 requestQuit 上的长注释。
        try {
          if (ev('return tostring(__bro_quit_restart or false)') === 'true') {
            requestQuit();
            return;
          }
        } catch { /* 老 WASM 无此字段 */ }
        // 设置菜单切「4.0弹幕(legacy)」后 Lua 置位；legacy 是加载期分支，
        // 必须整页重载才会重新按新模式加载符卡脚本。flush 存档后重载。
        // 已知遗留：这条重载在 eagler iframe 里同样会被判「document 被替换」
        // （需要启动器提供 restart/relaunch 命令才能真正修好），不在 #39 范围。
        try {
          if (ev('return tostring(__bro_reload_requested or false)') === 'true') {
            void storage.flush();
            location.reload();
            return;
          }
        } catch { /* 老 WASM 无此字段 */ }
        try { runtime.lock60 = ev('return tostring(__bro_web_lock60)') === 'true'; } catch { /* 老 WASM 无此字段 */ }
        // #37：对局中途回到原生主菜单（stage 'menu'，见 game/THlib/UI/title.lua:3）
        // → 上报 exit(returnToMenu)，启动器才会释放座位、房间回待准备态，从而
        // 允许重新开局。observeStage 内部有「本局离开过主菜单」的栅栏，避免
        // startMatch 刚下发时的误判。
        try {
          if (adapter?.multiStarted) {
            adapter.observeStage(ev('return tostring(stage.current_stage and stage.current_stage.stage_name or "")'));
          }
        } catch { /* 时序/老 WASM：不影响渲染 */ }
        runtime.fps = Math.round(fpsFrames * 1000 / (now - fpsSince));
        runtime.frameMs = (now - fpsSince) / Math.max(1, fpsFrames);
        runtime.maxGapMs = Math.max(runtime.maxGapMs || 0, dt);
        fpsSince = now; fpsFrames = 0;
      }
      requestAnimationFrame(frame);
    } catch (error) {
      runtime.state = 'error';
      adapter?._emit?.('error', { message: 'frame', error: String(error && error.message || error) });
      console.error(error);
    }
  }

  const flushNow = () => { void storage.flush(); };
  if (typeof addEventListener === 'function') {
    addEventListener('pagehide', flushNow);
    addEventListener('visibilitychange', () => { if (document.visibilityState === 'hidden') flushNow(); });
  }
  runtime.stop = () => { stopped = true; flushNow(); };
  requestAnimationFrame(frame);
  return runtime;
}
