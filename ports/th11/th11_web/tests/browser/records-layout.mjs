// Test-only Application probes; no diagnostic export is shipped in release.
import {readFileSync,readdirSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {createRequire} from 'node:module';
import {createServer} from 'node:http';
import assert from 'node:assert/strict';

const game=resolve(import.meta.dirname,'../../..'),workspace=process.env.EAGLER_WORKSPACE;
if(!workspace)throw Error('Set EAGLER_WORKSPACE to the maintainer workspace');
const require=createRequire(resolve(workspace,'eagler-touhou/package.json'));
const puppeteer=require('puppeteer-core');const {unzipSync}=require('fflate');
const root=resolve(game,'th11_web'),out=resolve(root,'artifacts/records-layout-browser');mkdirSync(out,{recursive:true});
const sdk=process.env.EMSDK||resolve(workspace,'th08/tools/emsdk');
const compiler=['install/emscripten/emcc.py','upstream/emscripten/emcc.py'].map(p=>resolve(sdk,p)).find(existsSync);
const env={...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),EMSDK:sdk,TEMP:out,TMP:out};
const compile=args=>{const p=spawnSync('python',[compiler,...args],{env,encoding:'utf8',windowsHide:true});if(p.status!==0)throw Error(p.stdout+p.stderr);};
const flags=['-O2','-std=c++17','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_THCRAP=1','-DTH_ENABLE_THPRAC=1','-DTH11_DEVELOPMENT_HARNESS=1','-DIMGUI_DISABLE_WIN32_FUNCTIONS','-I'+resolve(root,'cpp/third_party/imgui'),'--use-port=sdl3','--use-port=sdl3_ttf'];
const app=resolve(out,'application.o');compile([...flags,'-c',resolve(root,'cpp/sdl/Application.cpp'),'-o',app]);
const objectRoot=resolve(root,'artifacts/sdl3/objects');
const objects=readdirSync(objectRoot).filter(n=>n.endsWith('.o')&&!n.includes('Application_cpp')).map(n=>resolve(objectRoot,n));
compile([...flags,'--no-entry','-sDEFAULT_TO_CXX=1','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=web,worker','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=2097152','-sINITIAL_MEMORY=134217728','-sFILESYSTEM=1','-lidbfs.js','-sEXPORTED_RUNTIME_METHODS=FS,HEAPU8','-sEXPORTED_FUNCTIONS=_malloc,_free','-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','-sMIN_WEBGL_VERSION=2','-sMAX_WEBGL_VERSION=2',...objects,app,'-o',resolve(out,'probe.mjs')]);
const files=new Map([['/probe.mjs',resolve(out,'probe.mjs')],['/probe.wasm',resolve(out,'probe.wasm')],['/th11.dat',resolve(workspace,'prepared/th11-content/th11.data')],['/unifont.otf',resolve(workspace,'prepared/th11-localization-site/shared/unifont.otf')]]);
files.set('/eagler-host.mjs',resolve(root,'sdl-runtime/eagler-host.mjs'));
for(const name of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])files.set('/fonts/'+name,resolve(workspace,'th11/th11_web/assets/sdl-native/fonts',name));
const imported=unzipSync(readFileSync(resolve(process.env.USERPROFILE,'Downloads/th11-offline-lang-auto-dialogue-20261003.zip')));
const packs={};for(const [locale,lang] of [['zh-CN','lang_zh-hans'],['en-US','lang_en']]){packs[locale]=unzipSync(imported['games/th11/language/'+lang+'.zip']);for(const [path,bytes]of Object.entries(packs[locale]))files.set('/pack/'+locale+'/'+path,bytes);}
const music=readdirSync(resolve(workspace,'prepared/th11-content/music')).filter(n=>n.endsWith('.ogg')).map(n=>'/music/'+n);for(const path of music)files.set(path,resolve(workspace,'prepared/th11-content',path.slice(1)));
const server=createServer((req,res)=>{const p=new URL(req.url,'http://localhost').pathname;if(p==='/'){res.setHeader('Content-Type','text/html');res.end('<style>html,body{margin:0;background:black}canvas{width:640px;height:480px}</style><canvas id="canvas" width="640" height="480"></canvas>');return;}if(!files.has(p)){res.writeHead(404).end();return;}res.setHeader('Content-Type',p.endsWith('.mjs')?'text/javascript':p.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(typeof files.get(p)==='string'?readFileSync(files.get(p)):files.get(p));});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
const browserRoot=resolve(process.env.LOCALAPPDATA,'ms-playwright');
const executablePath=resolve(browserRoot,readdirSync(browserRoot).filter(n=>/^chromium-\d+$/.test(n)).sort().reverse()[0],'chrome-win64/chrome.exe');
let browser;
try{
 browser=await puppeteer.launch({executablePath,headless:true,args:['--no-sandbox','--enable-unsafe-swiftshader','--use-gl=angle','--use-angle=swiftshader']});
 for(const locale of ['zh-CN','en-US','ja-JP']){
  const page=await browser.newPage();await page.setViewport({width:800,height:600,hasTouch:true});const errors=[];page.on('pageerror',e=>errors.push(e.message));await page.goto('http://127.0.0.1:'+server.address().port);
  await page.evaluate(async ({locale,music,packPaths})=>{
   const {default:create}=await import('/probe.mjs');const c=window.core=window.Module=await create({canvas:document.querySelector('canvas'),instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('/probe.wasm'),imports).then(({instance,module})=>{window.native=instance.exports;ready(instance,module);return instance.exports;});}});c.eaglerOptions={thpracEnabled:true,thpracLocale:locale};c.eaglerControls={thpracKeyboardBits:0};
   for(const name of ['/th11.dat','/unifont.otf',...music,...['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'].map(n=>'/fonts/'+n)]){c.FS.mkdirTree(name.slice(0,name.lastIndexOf('/'))||'/');c.FS.writeFile(name,new Uint8Array(await fetch(name).then(r=>r.arrayBuffer())));}
   for(const path of packPaths){c.FS.mkdirTree('/'+path.slice(0,path.lastIndexOf('/')));c.FS.writeFile('/'+path,new Uint8Array(await fetch('/pack/'+locale+'/'+path).then(r=>r.arrayBuffer())));}
   window.error=()=>{const p=c._th11_error();return new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));};
   window.step=(n=1)=>{for(let i=0;i<n;++i)if(!c._th11_probe_platform_tick())throw Error(error());};
   window.key=(scan,n=1)=>{c._th11_key(scan,1);step(n);c._th11_key(scan,0);step(2);};
   window.state=()=>Array.from(new Float64Array(c.HEAPU8.buffer,c._th11_probe_practice_state(),20));
   c._th11_music_enabled(0);if(!c._th11_initialize())throw Error(error());step(120);
  },{locale,music,packPaths:Object.keys(packs[locale]||{})});
  await page.evaluate(()=>{if(!core._th11_probe_records())throw Error(error());step(30);key(208);key(208);key(208);key(44);step(30);if(core._th11_text_writes()<10)throw Error('Record rows not rasterized');});
  await page.screenshot({path:resolve(out,locale+'-records.png')});
  await page.evaluate(locale=>{
   const string=p=>new TextDecoder().decode(core.HEAPU8.subarray(p,core.HEAPU8.indexOf(0,p)));
   const base=string(core._th11_probe_spell_name(2,0)),variant=string(core._th11_probe_spell_name(3,1));
   if(locale!=='ja-JP'&&(base==='original'||variant!==base))throw Error('Imported spell table failed thcrap rank fallback');
   if(locale==='ja-JP'&&variant!=='original')throw Error('Japanese baseline changed');
   if(locale!=='ja-JP')for(let id=0;id<175;id++){
    const rank=id<162?(id+2)%4:4;
    if(string(core._th11_probe_spell_name(id,rank))==='original')throw Error('Missing translated Player Data spell '+id);
   }
   if(!core._th11_probe_practice_menu(0,0,1))throw Error(error());step(5);
   native.sdl_touch_controls(1,0,0,1,0,0);step(15);
   if(state()[14])throw Error('Touch ESC failed to cancel Practice');
   if(!core._th11_probe_practice_menu(0,0,1))throw Error(error());step(5);key(44);step(120);
   native.sdl_touch_options(1,0,1);native.sdl_touch_controls(1,0,0,1,0,0);step(4);
   if(!(core._th11_probe_gameplay_held()&1))throw Error('Practice owner captured enabled touch fire');
   native.sdl_touch_controls(1,0,0,2,0,0);step(15);
   if(!core._th11_probe_paused())throw Error('Touch ESC failed to pause with fire ON');
  },locale);
  assert.deepEqual(errors,[]);console.log(locale+': rows, imported spell variants, touch ESC cancel/pause passed');await page.close();
 }
}finally{await browser?.close();await new Promise(r=>server.close(r));}
