// Loaded before the game module so initialization failures and Emscripten's
// stderr survive even when the WebAssembly instance cannot finish starting.
(() => {
  'use strict';
  if (globalThis.__eaglerCrashReporterV1) return;
  const scriptUrl = document.currentScript?.src || location.href;
  const game = /\bth(?:12|15)\b/.exec(location.pathname)?.[0] || '';
  const multiplayer = location.pathname.includes('/multiplayer/');
  const generation = /\/([a-f0-9]{64})\//.exec(location.pathname)?.[1] || '';
  const logs = [], events = [], samples = [];
  let core, context, report, build, stopped = false;
  const append = (list, value, limit) => { list.push(value); if (list.length > limit) list.shift(); };
  const trim = value => String(value).slice(0, 12000).replace(/https?:\/\/[^\s"<>]+/g, url => {
    try { const parsed = new URL(url); parsed.search = ''; parsed.hash = ''; return parsed.href; } catch { return url; }
  });
  function plain(value, depth = 0, seen = new Set()) {
    if (value == null || typeof value === 'boolean' || typeof value === 'number') return value;
    if (typeof value !== 'object') return trim(value);
    if (seen.has(value)) return '[circular]';
    if (depth > 5) return '[depth limit]';
    seen.add(value);
    if (value instanceof Error || Object.prototype.toString.call(value).endsWith('Error]'))
      return {name:trim(value.name), message:trim(value.message), stack:trim(value.stack || ''), cause:plain(value.cause, depth + 1, seen)};
    if (ArrayBuffer.isView(value)) return {type:value.constructor.name, bytes:value.byteLength};
    if (Array.isArray(value)) return value.slice(0, 80).map(v => plain(v, depth + 1, seen));
    const out = {};
    for (const key of Object.keys(value).slice(0, 60)) {
      if (/token|password|secret|credential|member|netplayUrl|replayBytes|^bytes$/i.test(key)) continue;
      try { out[key] = plain(value[key], depth + 1, seen); } catch { out[key] = '[unavailable]'; }
    }
    return out;
  }
  function errorValue(value) {
    return plain(value instanceof Error ? value : new Error(typeof value === 'string' ? value : String(value)));
  }
  for (const level of ['warn', 'error']) {
    const original = console[level];
    console[level] = function(...args) {
      try { append(logs, {time:new Date().toISOString(), level, values:args.slice(0, 8).map(v => plain(v))}, 100); } catch {}
      return original.apply(this, args);
    };
  }
  function sample() {
    if (stopped || !core) return;
    const value = {time:new Date().toISOString()};
    try {
      const heap = core.HEAPU8;
      if (heap) value.wasmMemoryBytes = heap.byteLength;
      const scalar = name => typeof core[name] === 'function' ? core[name]() : undefined;
      const words = (name, count, signed = false) => {
        if (!heap || typeof core[name] !== 'function') return undefined;
        const pointer = core[name]() >>> 0;
        if (!pointer || pointer % 4 || pointer + count * 4 > heap.length) return undefined;
        return Array.from((signed ? new Int32Array(heap.buffer) : new Uint32Array(heap.buffer)).subarray(pointer / 4, pointer / 4 + count));
      };
      const prefix = game || (core._th15_frame ? 'th15' : 'th12');
      value.frame = scalar('_' + prefix + '_frame');
      value.phase = scalar('_' + prefix + '_phase');
      value.netplayStatus = words('_' + prefix + '_mp_status', 24);
      const rollback = words('_' + prefix + '_mp_rollback_status', prefix === 'th12' ? 8 : 9);
      if (rollback) value.rollback = prefix === 'th12'
        ? {version:rollback[0], corrections:rollback[1], replayedFrames:rollback[2], committedFrames:rollback[3], correcting:!!rollback[4], requestedFrame:rollback[5], frontier:rollback[6], worldReady:!!rollback[7]}
        : {version:rollback[0], enabled:!!rollback[1], corrections:rollback[2], replayedFrames:rollback[3], lastDepth:rollback[4], peakDepth:rollback[5], replayRemaining:rollback[6], fenceFrame:rollback[7], presentationFps:rollback[8]/100};
      if (context) value.gameState = plain(context());
      if (prefix === 'th12' && value.gameState?.session) {
        if (typeof core._th12_stage_flow === 'function' && typeof core._th12_stage_flow_size === 'function') {
          const pointer = core._th12_stage_flow() >>> 0, length = core._th12_stage_flow_size() >>> 0;
          if (length && length <= 65536 && pointer + length <= heap?.length)
            value.stageFlow = plain(JSON.parse(new TextDecoder().decode(heap.subarray(pointer, pointer + length))));
        }
        const pointer = scalar('_th12_hud') >>> 0, length = Math.min(scalar('_th12_hud_size') >>> 0, 64);
        if (pointer && length && pointer + length * 4 <= heap?.length)
          value.hud = Array.from(new Float32Array(heap.buffer, pointer, length));
      }
    } catch (e) { value.sampleError = errorValue(e); }
    append(samples, value, 30);
  }
  function hostUI(target) {
    if (target.__eaglerCrashReportHostV1) return target.__eaglerCrashReportHostV1;
    const doc = target.document;
    let panel, latest, detail, heading, description, summary, comment, download, status;
    const node = (tag, text, parent) => { const e=doc.createElement(tag); if (text) e.textContent=text; parent?.append(e); return e; };
    function show(value) {
      latest = value;
      if (!doc.body) { doc.addEventListener('DOMContentLoaded', () => show(latest), {once:true}); return; }
      if (!panel) {
        panel = node('dialog', '', doc.body); panel.id='eagler-crash-report';
        panel.style.cssText='position:fixed;inset:0;z-index:2147483647;display:flex;align-items:center;justify-content:center;width:100%;height:100%;max-width:none;max-height:none;margin:0;border:0;padding:12px;box-sizing:border-box;background:#000b;color:#eee;font:15px/1.5 system-ui,sans-serif;text-align:left';
        const box=node('section','',panel); box.setAttribute('role','dialog'); box.setAttribute('aria-modal','true'); box.setAttribute('aria-labelledby','eagler-crash-heading');
        box.style.cssText='box-sizing:border-box;width:min(820px,100%);max-height:92vh;overflow:auto;padding:20px;border:1px solid #777;border-radius:16px;background:#191817;box-shadow:0 12px 60px #0009';
        heading=node('h2','游戏发生错误',box); heading.id='eagler-crash-heading'; heading.style.margin='0 0 8px';
        description=node('p','',box); description.style.margin='8px 0';
        summary=node('p','',box);summary.style.cssText='margin:8px 0;color:#bbb;overflow-wrap:anywhere';
        node('p','请下载错误报告并发送给维护者。报告包含崩溃堆栈、最近日志、版本和运行状态，便于定位问题。',box);
        detail=node('pre','',box); detail.style.cssText='white-space:pre-wrap;overflow-wrap:anywhere;max-height:35vh;overflow:auto;background:#080808;padding:12px;border-radius:8px;font:12px/1.5 monospace;color:#ffd6cc';
        const label=node('label','崩溃前发生了什么（可选）',box); comment=node('textarea','',label); comment.rows=2;
        comment.style.cssText='display:block;box-sizing:border-box;width:100%;margin:6px 0 12px;padding:8px;background:#222;color:#fff;border:1px solid #777;border-radius:6px;font:inherit';
        const actions=node('div','',box); actions.style.cssText='display:flex;flex-wrap:wrap;gap:8px';
        function button(text, action, primary=false) { const b=node('button',text,actions); b.type='button'; b.style.cssText='min-height:44px;padding:10px 16px;border:1px solid #777;border-radius:8px;color:white;font:inherit;cursor:pointer;background:'+(primary?'#b33f31':'#34312f'); b.onclick=action; return b; }
        download=button('下载错误报告 (.json)', () => {
          try {
            latest.userComment=comment.value.slice(0, 4000);
            const url=target.URL.createObjectURL(new Blob([JSON.stringify(latest,null,2)+'\n'],{type:'application/json;charset=utf-8'}));
            const link=node('a','',doc.body); link.href=url; link.download=`eagler-${latest.game||'game'}-crash-${latest.id}.json`; link.click(); link.remove();
            target.setTimeout(()=>target.URL.revokeObjectURL(url),60000); status.textContent='已请求下载，请将 JSON 报告发给维护者。';
          } catch (e) { status.textContent='下载失败，请复制错误详情：'+String(e); }
        },true);
        button('复制错误详情', async () => {
          try { latest.userComment=comment.value.slice(0,4000); await target.navigator.clipboard.writeText(JSON.stringify(latest,null,2)); status.textContent='已复制完整报告。'; }
          catch { status.textContent='复制失败，请使用“下载错误报告”。'; }
        });
        const collapse=()=>{panel.close?.();panel.style.display='none';reopen.style.display='block';};
        button('收起提示', collapse);panel.addEventListener('cancel',event=>{event.preventDefault();collapse();});
        status=node('p','',box); status.setAttribute('role','status');
        const reopen=node('button','下载错误报告',doc.body); reopen.type='button';
        reopen.style.cssText='display:none;position:fixed;right:12px;bottom:12px;z-index:2147483647;padding:12px;border:1px solid #ccc;border-radius:10px;background:#a33;color:#fff;font:15px system-ui';
        reopen.onclick=()=>{reopen.style.display='none';show(latest);};
        // The host can open its disconnect dialog after the fatal callback.
        // Native dialogs share a top layer above all CSS z-index values.
        new MutationObserver(records=>{
          if(panel.open&&records.some(r=>r.target!==panel&&r.target.tagName==='DIALOG'&&r.target.open))
            target.setTimeout(()=>{if(panel.open){panel.close();panel.showModal();download.focus();}},0);
        }).observe(doc.body,{attributes:true,subtree:true,attributeFilter:['open']});
      }
      description.textContent=`${(value.game||'游戏').toUpperCase()} · ${value.multiplayer?'多人':'单人'} · ${value.createdAt} · 报告 ${value.id}`;
      const last=value.samples.at(-1),stage=last?.gameState?.session?.stage;
      summary.textContent=[value.runtime.build?.version,stage&&`第 ${stage} 面`,last?.frame!==undefined&&`帧 ${last.frame}`,value.wasmMemoryBytes&&`WASM 内存 ${(value.wasmMemoryBytes/1048576).toFixed(1)} MiB`].filter(v=>v!==false&&v!==undefined&&v!==null&&v!=='').join(' · ');
      const messages=value.errors.map(e=>`[${e.source}] ${e.error.stack||e.error.message}`).join('\n\n');
      detail.textContent=messages.slice(0,16000)+(value.logs.length?'\n\n最近错误日志：\n'+value.logs.slice(-12).map(l=>l.values.map(v=>typeof v==='string'?v:JSON.stringify(v)).join(' ')).join('\n').slice(-8000):'');
      if(!panel.isConnected)doc.body.append(panel);
      if(panel.open)panel.close();panel.style.display='flex';panel.showModal?.();download.focus();
    }
    target.__eaglerCrashReportHostV1={show}; return target.__eaglerCrashReportHostV1;
  }
  function fatal(error, fields={}) {
    try {
      stopped=true;
      if (!report) {
        const when=new Date();
        report={schema:'eagler-touhou/crash-report/1',reporterVersion:1,id:when.toISOString().replace(/[^0-9TZ]/g,'')+'-'+Math.random().toString(16).slice(2,8),createdAt:when.toISOString(),game,multiplayer,generation,
          runtime:{url:location.origin+location.pathname,build:plain(build)},
          environment:{userAgent:navigator.userAgent,language:navigator.language,languages:navigator.languages,platform:navigator.platform,hardwareConcurrency:navigator.hardwareConcurrency,deviceMemoryGiB:navigator.deviceMemory,online:navigator.onLine,visibility:document.visibilityState,viewport:{width:innerWidth,height:innerHeight,pixelRatio:devicePixelRatio},timezone:Intl.DateTimeFormat().resolvedOptions().timeZone},
          errors:[],logs:[],events:[],samples:[]};
      }
      const entry={time:new Date().toISOString(),source:fields.source||'runtime-fatal',error:errorValue(error)};
      if (!report.errors.some(e=>e.source===entry.source&&e.error.message===entry.error.message)) append(report.errors,entry,12);
      report.logs=logs.slice();report.events=events.slice();report.samples=samples.slice();
      report.runtime.build=plain(build);
      if (core?.HEAPU8) report.wasmMemoryBytes=core.HEAPU8.byteLength;
      const memory=performance.memory; if(memory)report.browserMemory={usedBytes:memory.usedJSHeapSize,totalBytes:memory.totalJSHeapSize,limitBytes:memory.jsHeapSizeLimit};
      const transport=globalThis.__eaglerPeerTransport;
      if(transport)report.transport=plain({route:transport.route,connectionState:transport.connectionState,
        receivedQueued:transport.received?.length-(transport.receivedHead||0),channelEvents:transport.channelEvents,
        peers:transport.peers&&Array.from(transport.peers).slice(0,2).map(([peer,p])=>({peer,
          connectionState:p.pc?.connectionState,iceConnectionState:p.pc?.iceConnectionState,sctpState:p.pc?.sctp?.state,
          input:{readyState:p.inputDc?.readyState,bufferedAmount:p.inputDc?.bufferedAmount},
          control:{readyState:p.controlDc?.readyState,bufferedAmount:p.controlDc?.bufferedAmount},route:p.adaptive?.snapshot?.()}))});
      let host=window;try{if(parent!==window&&parent.location.origin===location.origin)host=parent;}catch{}
      try{hostUI(host).show(report);}catch{hostUI(window).show(report);}
    } catch {}
  }
  function bindCore(module, details={}) {
    core=module;context=details.context; if(details.build)build={...build,...details.build};
    try { const before=module.onAbort; module.onAbort=reason=>{fatal(new Error(String(reason)),{source:'wasm-abort'});if(typeof before==='function')before(reason);}; } catch {}
    sample();
  }
  globalThis.__eaglerCrashReporterV1={fatal,bindCore,sample,report:()=>report};
  addEventListener('error',event=>{
    if(event.error||event.message)fatal(event.error||new Error(event.message),{source:'uncaught-error'});
    else if(event.target?.tagName==='SCRIPT')fatal(new Error('游戏脚本加载失败：'+(event.target.src||'')),{source:'script-load'});
  },true);
  addEventListener('unhandledrejection',event=>fatal(event.reason,{source:'unhandled-rejection'}));
  function observe() {
    for(const element of document.querySelectorAll('#error,#notice')) {
      const check=()=>{const message=element.textContent.trim();if(message&&(element.id==='error'||/^运行中断[：:]/.test(message)))fatal(new Error(message),{source:'native-fatal'});};
      new MutationObserver(check).observe(element,{childList:true,subtree:true,characterData:true});check();
    }
    document.querySelector('canvas')?.addEventListener('webglcontextlost',()=>fatal(new Error('WebGL 图形上下文丢失'),{source:'webgl-context-lost'}));
  }
  if(document.readyState==='loading')document.addEventListener('DOMContentLoaded',observe,{once:true});else observe();
  addEventListener('message',event=>{
    if(event.origin!==location.origin||event.source!==window&&event.source!==parent)return;
    const data=event.data;
    if(data?.protocol!=='eagler-touhou/1')return;
    if(data.command==='configure')append(events,{time:new Date().toISOString(),type:'configure',options:plain(data.options),language:data.language},60);
  });
  // postMessage to the parent does not dispatch in the child; retain emitted
  // health events at the parent without collecting other Runtime instances.
  try { if(parent!==window&&parent.location.origin===location.origin) {
    const source=window, receive=event=>{if(event.source!==source||event.origin!==location.origin)return;const d=event.data;if(d?.protocol==='eagler-touhou/1'&&['frame-health','audio-health','runtime-info','error','peer-connection'].includes(d.event))append(events,{time:new Date().toISOString(),...plain(d)},60);};
    parent.addEventListener('message',receive); addEventListener('pagehide',()=>parent.removeEventListener('message',receive),{once:true});
  }} catch {}
  setInterval(()=>{if(!core&&globalThis.Module)bindCore(globalThis.Module);else sample();},1000);
  fetch(new URL('./manifest.json',scriptUrl)).then(r=>r.ok?r.json():null).then(value=>{if(value){build={...value,...build};if(report)report.runtime.build=plain(build);}}).catch(()=>{});
  fetch(new URL('./crash-code-identities.json',scriptUrl)).then(r=>r.ok?r.json():null).then(value=>{
    if(Array.isArray(value?.files)){build={...build,codeIdentities:value.files.filter(f=>/\.wasm$|^(?:core|th15)\.mjs$/.test(f.path))};if(report)report.runtime.build=plain(build);}
  }).catch(()=>{});
})();
