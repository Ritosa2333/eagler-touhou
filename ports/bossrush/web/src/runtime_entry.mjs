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
  if (adapter) {
    adapter.ccall = ccall;
    adapter.Module = Module;
  }
  globalThis.__broModule = Module;
  globalThis.__broEval = ev;

  note('loading saves...');
  const storage = createStorage(Module);
  // B2 铁律：launch.lua 在 bro_boot() 当场读 User/setting，loadAll 必须先行。
  const loadStats = await storage.loadAll();
  if (adapter) adapter.storage = storage;

  note('booting lua...');
  const boot = ccall('bro_boot', 'number', [], []);
  if (boot !== 0) throw new Error(`bro_boot failed: ${boot} ${ccall('bro_stats', 'string', [], [])}`);

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
  await renderer.requestAll({});
  for (const [name, path, size] of FONT_REGISTRY) {
    renderer.text.registerFont(name, { kind: 8, path, size });
  }
  await renderer.text.prepare();
  audio.flush();
  audio.syncResources();
  if (params.get('audioNoWarm') !== '1') {
    try { await audio.warm({ concurrency: 4 }); } catch (error) { console.warn('audio warm failed', error); }
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

  renderer.loadResourceTable();
  await renderer.requestAll({});

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
  let lastCanvasW = canvas.width;
  let lastCanvasH = canvas.height;
  let stopped = false;
  const stepMs = 1000 / 60;

  function frame(now) {
    if (stopped) return;
    try {
      let dt = now - last;
      last = now;
      if (dt > 100) dt = 100;
      accumulator += dt;
      let stepped = 0;
      while (accumulator >= stepMs && stepped < 4) {
        input.syncHeld();
        ccall('bro_frame', 'number', [], []);
        storage.tick(++runtime.frames);
        accumulator -= stepMs;
        stepped++;
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

      if (!firstFrameSent) {
        firstFrameSent = true;
        adapter?.notifyFirstFrame?.(adapter.launchSerial);
      }
      fpsFrames++;
      if (now - fpsSince >= 500) {
        try { runtime.lock60 = ev('return tostring(__bro_web_lock60)') === 'true'; } catch { /* 老 WASM 无此字段 */ }
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
