// Local developer preview: serve only the module and explicitly listed private assets.
import {createServer} from 'node:http';
import {readFile,readdir} from 'node:fs/promises';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'../th15_web');
const build=resolve(root,'artifacts/thprac-purple-browser-check');
const assets=resolve(root,'artifacts/thprac-private-assets');
const fonts=(await readdir(resolve(assets,'fonts'))).filter(n=>n.endsWith('.bin'));
const files=new Map([['/module.mjs',resolve(build,'th15-application.mjs')],['/th15-application.wasm',resolve(build,'th15-application.wasm')],['/keyboard.mjs',resolve(root,'sdl-runtime/keyboard.mjs')],['/th15.dat',resolve(assets,'th15.dat')],['/unifont.otf',resolve(assets,'unifont.otf')],...fonts.map(n=>['/fonts/'+n,resolve(assets,'fonts',n)])]);
const html=`<!doctype html><html lang="zh-CN"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no"><title>TH15 紫 THPrac 本地测试</title>
<style>*{box-sizing:border-box}html,body{margin:0;background:#000;color:#eee;width:100%;height:100%;overflow:hidden;font:14px system-ui}body{display:grid;place-items:center}canvas{width:min(100vw,133.333vh);height:min(75vw,100vh);touch-action:none;outline:none}#status{position:fixed;top:45%;padding:20px;background:#171717}nav{position:fixed;right:8px;top:8px;display:flex;gap:5px;flex-wrap:wrap;max-width:95vw}button{background:#222b;color:white;border:1px solid #666;border-radius:6px;padding:8px;touch-action:none}#hint{position:fixed;bottom:5px;right:8px;color:#aaa;font-size:12px;pointer-events:none}</style>
<canvas id="canvas" width="640" height="480" tabindex="0"></canvas><div id="status">正在加载 TH15 和共享字体…</div><nav><button data-scan="1">ESC</button><button data-scan="44">Z 确定／开火</button><button data-scan="45">X</button><button data-scan="200">↑</button><button data-scan="208">↓</button><button data-scan="203">←</button><button data-scan="205">→</button><button data-scan="88">F12</button><button data-scan="15">Tab</button><button data-scan="14">Backspace</button><button id="full">全屏</button></nav><div id="hint">Practice 进入紫 THPrac；F12 高级菜单；Tab 详细信息。此测试入口关闭音乐。</div>
<script type="module">
import create from '/module.mjs';import {scanCodes} from '/keyboard.mjs';
const canvas=document.querySelector('canvas'),status=document.querySelector('#status');
try{
 const core=await create({canvas,eaglerOptions:{thpracEnabled:true,thpracLocale:new URLSearchParams(location.search).get('locale')||'zh-CN'}});window.Module=core;
 core.FS.mkdirTree('/fonts');core.FS.mkdirTree('/save/replay');core.FS.mkdirTree('/save/autosave');
 for(const p of ${JSON.stringify(['/th15.dat','/unifont.otf',...fonts.map(n=>'/fonts/'+n)])}){const response=await fetch(p);if(!response.ok)throw Error('资源读取失败 '+p);core.FS.writeFile(p,new Uint8Array(await response.arrayBuffer()));}
 core.resetBrowserKeyboard=()=>{};core._th15_music_enabled(0);core._th15_render_scale(1);core._th15_touch_options(1,0,1,0,0);
 if(!core._th15_initialize())throw Error(core.UTF8ToString?core.UTF8ToString(core._th15_error()):'TH15 初始化失败');
 core.onGameFrame=ok=>{if(!ok){status.hidden=false;const p=core._th15_error();status.textContent=new TextDecoder().decode(core.HEAPU8.subarray(p,core.HEAPU8.indexOf(0,p)));}};
 const clear=()=>core._th15_keys_clear();
 for(const type of ['keydown','keyup'])window.addEventListener(type,e=>{const scan=scanCodes[e.code];if(!scan||e.altKey||e.metaKey)return;e.preventDefault();core._th15_key(scan,+(type==='keydown'));},{capture:true});
 window.addEventListener('blur',()=>{clear();core._th15_touch_cancel();});
 document.addEventListener('visibilitychange',()=>{clear();core._th15_touch_cancel();core._th15_loop_pause(+document.hidden);});
 for(const [name,type] of [['pointerdown',0],['pointermove',1],['pointerup',2],['pointercancel',2]])canvas.addEventListener(name,e=>{const r=canvas.getBoundingClientRect();const x=(e.clientX-r.left)/r.width,y=(e.clientY-r.top)/r.height;e.preventDefault();if(type===0){canvas.focus();canvas.setPointerCapture(e.pointerId);}if(e.pointerType==='mouse')core._th15_thprac_mouse(type===0?1:type===2?2:0,x*640,y*480);else if(name==='pointercancel')core._th15_touch_cancel();else core._th15_touch(type,e.pointerId,x,y);});
 for(const b of document.querySelectorAll('[data-scan]')){b.addEventListener('pointerdown',e=>{e.preventDefault();b.setPointerCapture(e.pointerId);core._th15_key(+b.dataset.scan,1);});for(const name of ['pointerup','pointercancel','lostpointercapture'])b.addEventListener(name,e=>{e.preventDefault();core._th15_key(+b.dataset.scan,0);});}
 document.querySelector('#full').onclick=()=>document.documentElement.requestFullscreen();status.hidden=true;canvas.focus();core._th15_loop_start();
}catch(e){status.textContent=String(e);console.error(e);}
</script></html>`;
const server=createServer(async(req,res)=>{try{const p=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');res.setHeader('Cache-Control','no-store');if(p==='/'){res.setHeader('Content-Type','text/html; charset=utf-8');res.end(html);return;}const file=files.get(p);if(!file){res.writeHead(404).end();return;}res.setHeader('Content-Type',p.endsWith('.mjs')?'text/javascript':p.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(500).end(String(e));}});
const port=Number(process.env.TH15_PREVIEW_PORT||8147);
server.listen(port,'127.0.0.1',()=>console.log('TH15 purple THPrac preview: http://127.0.0.1:'+port+'/'));
