// Test-only Application probes; no diagnostic export is shipped in release.
import {readFileSync,readdirSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {spawnSync} from 'node:child_process';
import {createRequire} from 'node:module';
import {createServer} from 'node:http';
import assert from 'node:assert/strict';
import {sections} from '../../sdl-runtime/practice-sections.mjs';
const game=resolve(import.meta.dirname,'../../..'),workspace=process.env.EAGLER_WORKSPACE;
if(!workspace)throw Error('Set EAGLER_WORKSPACE to the maintainer workspace');
const require=createRequire(resolve(workspace,'eagler-touhou/package.json'));
const puppeteer=require('puppeteer-core');
const root=resolve(game,'th11_web'),out=resolve(root,'artifacts/thprac-browser');mkdirSync(out,{recursive:true});
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
const music=readdirSync(resolve(workspace,'prepared/th11-content/music')).filter(n=>n.endsWith('.ogg')).map(n=>'/music/'+n);for(const path of music)files.set(path,resolve(workspace,'prepared/th11-content',path.slice(1)));
const server=createServer((req,res)=>{const p=new URL(req.url,'http://localhost').pathname;if(p==='/'){res.setHeader('Content-Type','text/html');res.end('<style>html,body{margin:0;background:black}canvas{width:640px;height:480px}</style><canvas id="canvas" width="640" height="480"></canvas>');return;}if(!files.has(p)){res.writeHead(404).end();return;}res.setHeader('Content-Type',p.endsWith('.mjs')?'text/javascript':p.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(readFileSync(files.get(p)));});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
const browserRoot=resolve(process.env.LOCALAPPDATA,'ms-playwright');
const executablePath=resolve(browserRoot,readdirSync(browserRoot).filter(n=>/^chromium-\d+$/.test(n)).sort().reverse()[0],'chrome-win64/chrome.exe');
let browser;
try{
 browser=await puppeteer.launch({executablePath,headless:true,args:['--no-sandbox','--enable-unsafe-swiftshader','--use-gl=angle','--use-angle=swiftshader']});
 for(const locale of ['zh-CN','en-US','ja-JP']){
  const page=await browser.newPage();await page.setViewport({width:800,height:600,hasTouch:true});const errors=[];page.on('pageerror',e=>errors.push(e.message));await page.goto('http://127.0.0.1:'+server.address().port);
  await page.evaluate(async ({locale,music})=>{
   const {default:create}=await import('/probe.mjs');const c=window.core=window.Module=await create({canvas:document.querySelector('canvas'),instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('/probe.wasm'),imports).then(({instance,module})=>{window.native=instance.exports;ready(instance,module);return instance.exports;});}});c.eaglerOptions={thpracEnabled:true,thpracLocale:locale};c.eaglerControls={thpracKeyboardBits:0};
   for(const name of ['/th11.dat','/unifont.otf',...music,...['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'].map(n=>'/fonts/'+n)]){c.FS.mkdirTree(name.slice(0,name.lastIndexOf('/'))||'/');c.FS.writeFile(name,new Uint8Array(await fetch(name).then(r=>r.arrayBuffer())));}
   window.error=()=>{const p=c._th11_error();return new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));};
   window.step=(n=1)=>{for(let i=0;i<n;++i)if(!c._th11_probe_platform_tick())throw Error(error());};
   window.key=(scan,n=1)=>{c._th11_key(scan,1);step(n);c._th11_key(scan,0);step(2);};
   window.state=()=>Array.from(new Float64Array(c.HEAPU8.buffer,c._th11_probe_practice_state(),20));
   c._th11_music_enabled(0);if(!c._th11_initialize())throw Error(error());step(120);
  },{locale,music});
  const result=await page.evaluate(()=>{
   // Original menu chain, not the direct-stage diagnostic entry.
   key(208);key(44);step(60);key(44);step(30);key(44);step(30);key(44);step(50);
   if(!state()[14])throw Error('Original Practice/character/partner chain did not open native menu: '+Array.from(new Int32Array(core.HEAPU8.buffer,core._th11_probe_title_state(),4)));
   step(4);return state();
  });
  assert.equal(result[14],1);await page.screenshot({path:resolve(out,locale+'-practice.png')});
  await page.evaluate(()=>{key(45);step(15);if(state()[14])throw Error('Cancel left practice owner active');});
  for(const viewport of [{width:390,height:844},{width:844,height:390},{width:1024,height:768}]){
   await page.setViewport({...viewport,hasTouch:true});
   await page.evaluate(async locale=>{
    const {directTouch}=await import('/eagler-host.mjs');const canvas=document.querySelector('canvas');canvas.style.width='min(100vw, calc(100vh * 4 / 3))';canvas.style.height='auto';native.sdl_touch_options(1,0,1);
    if(!core._th11_probe_practice_menu(0,0,1))throw Error(error());step(4);
    const pending=()=>Array.from(new Float64Array(core.HEAPU8.buffer,core._th11_probe_practice_pending(),14));
    const initial=pending();initial[5]=9;initial[7]=80;const pointer=core._malloc(initial.length*8);try{new Float64Array(core.HEAPU8.buffer,pointer,initial.length).set(initial);if(!core._th11_practice_configure(pointer,initial.length))throw Error('Mobile test config rejected');}finally{core._free(pointer);}step(3);
    const touch=(type,x,y,id=-100)=>{const r=canvas.getBoundingClientRect();directTouch(native,canvas,{type,id,x:(r.left+x/640*r.width)/innerWidth,y:(r.top+y/480*r.height)/innerHeight},{width:innerWidth,height:innerHeight});step(2);};
    const left=locale==='en-US'?108:locale==='ja-JP'?138:158,top=locale==='en-US'?90:80;
    // Direct touch must change the actual ImGui slider, not merely the pointer.
    const before=pending()[5];touch('down',left+20,top+121);touch('move',left+150,top+121);touch('up',left+150,top+121);step(4);
    if(pending()[5]===before||!state()[14]||state()[15])throw Error('Mobile life slider failed or accepted menu accidentally');
    touch('down',left+30,top+69);touch('up',left+30,top+69);if(!core._th11_probe_practice_popup())throw Error('Mobile stage popup did not open');touch('down',left+30,top+121);touch('up',left+30,top+121);step(3);
    if(pending()[1]===0||!state()[14]||state()[15])throw Error('Mobile stage combo failed or accepted menu accidentally: '+JSON.stringify({pending:pending(),run:state()}));
    touch('down',left+30,top+173);native.sdl_touch_cancel();step(3);const cancelled=pending()[7];touch('move',left+160,top+173);if(pending()[7]!==cancelled)throw Error('Cancelled mobile pointer kept dragging');
    touch('down',30,30);touch('down',50,30,-101);touch('up',30,30);touch('up',50,30,-101);step(4);if(state()[14])throw Error('Mobile two-finger cancel failed');
    native.sdl_touch_controls(1,0,0,0,0,0);
    if(!core._th11_probe_practice_menu(0,0,1))throw Error(error());step(4);touch('down',30,30);touch('up',30,30);step(120);if(!state()[15])throw Error('Mobile outside-window confirm failed');
    if(!(core._th11_probe_gameplay_held()&1))throw Error('Enabled mobile fire was captured after Practice acceptance');
    core._th11_keys_clear();step(4);if(!(core._th11_probe_gameplay_held()&1))throw Error('Lifecycle clear lost toggled mobile fire');
    key(1);step(20);key(19);step(120);if(!(core._th11_probe_gameplay_held()&1))throw Error('Practice retry lost toggled mobile fire');
    native.sdl_touch_controls(0,0,0,0,0,0);step(4);if(core._th11_probe_gameplay_held()&1)throw Error('Disabled mobile fire still shoots');
    if(!core._th11_return_title())throw Error(error());step(3);
   },locale).catch(async error=>{await page.screenshot({path:resolve(out,locale+'-mobile-failure.png')});throw error;});
  }
  console.log(locale+': portrait/landscape/tablet direct-touch slider, stage combo and pointer cancellation passed');
  await page.setViewport({width:800,height:600,hasTouch:true});
  for(let shot=0;shot<6;++shot){
   const run=await page.evaluate(shot=>{
    if(!core._th11_probe_practice_menu(Math.floor(shot/3),shot%3,1))throw Error(error());step(3);
    const values=[1,0,shot%2?5:10101,0,0,7,2,shot===3?96:80,123,0,60000,123450,shot===4?3:0,1],pointer=core._malloc(values.length*8);
    try{new Float64Array(core.HEAPU8.buffer,pointer,values.length).set(values);if(!core._th11_practice_configure(pointer,values.length))throw Error('Config rejected');}finally{core._free(pointer);}
    step(3);key(44);step(120);const s=state();if(!s[15]||s[16]!==values[7]||s[2]!==values[2])throw Error('Native accept/run economy mismatch');
    // Original Pause -> R retry must preserve the run, not pending menu edits.
    key(1);step(20);key(19);step(20);if(!state()[15]||state()[5]!==7||state()[6]!==2)throw Error('Native pause retry lost practice parameters');
    if(!core._th11_probe_practice_save())throw Error('Custom replay save failed');
    const replay=core.HEAPU8.slice(core._th11_probe_practice_replay_data(),core._th11_probe_practice_replay_data()+core._th11_probe_practice_replay_size());if(!new TextDecoder().decode(replay).includes('PRAC'))throw Error('Saved replay omitted PRAC');
    if(!core._th11_probe_practice_play())throw Error(error());if(!state()[15]||state()[5]!==7)throw Error('Replay failed to restore practice owner');
    for(const [carrier,window,label] of [[1,1,'Backspace'],[1<<8,2,'Tab'],[1<<9,4,'F12']]){
     const before=core._th11_probe_practice_windows();core.eaglerControls.thpracKeyboardBits=carrier;step();core.eaglerControls.thpracKeyboardBits=0;step();
     if(!((before^core._th11_probe_practice_windows())&window))throw Error(label+' carrier did not toggle its native window');
    }
    return s;
   },shot);
   await page.screenshot({path:resolve(out,locale+'-shot'+shot+'-overlays.png')});
   assert.equal(run[5],7);assert.equal(run[6],2);
   await page.evaluate(()=>{core.eaglerControls.thpracKeyboardBits=1<<9;step();core.eaglerControls.thpracKeyboardBits=0;step();if(!core._th11_return_title())throw Error(error());step(3);if(state()[15])throw Error('Exit leaked live practice');});
  }
  await page.evaluate(()=>{
   const pose=()=>Array.from(new Int32Array(core.HEAPU8.buffer,core._th11_probe_function_state(),5));
   const start=(character,partner)=>{
    native.sdl_touch_controls(0,0,0,0,0,0);
    if(!core._th11_probe_practice_menu(character,partner,1))throw Error(error());step(5);
    const values=[1,0,6,0,0,9,0,80,0,0,50000,0,0,1],p=core._malloc(values.length*8);
    try{new Float64Array(core.HEAPU8.buffer,p,values.length).set(values);if(!core._th11_practice_configure(p,values.length))throw Error('C test config');}finally{core._free(p);}
    step(3);key(44);step(120);native.sdl_touch_options(1,0,1);native.sdl_touch_controls(1,0,0,0,0,0);step(4);
    if(!pose()[3]||pose()[4])throw Error('C test requires actual active boss and no dialogue');
   };
   start(0,0);
   core._th11_probe_function_pose(0);key(46);if(pose()[1]>=99)throw Error('C must not teleport away from an edge');
   core._th11_probe_function_pose(-1);key(46);if(pose()[1]!==99)throw Error('C failed left native gap activation');
   step(25);if(pose()[0]<=0)throw Error('Left gap did not cross the playfield');step(30);
   core._th11_probe_function_pose(1);key(46);if(pose()[1]!==100)throw Error('C failed right native gap activation');
   step(25);if(pose()[0]>=0)throw Error('Right gap did not cross the playfield');
   if(!core._th11_return_title())throw Error(error());step(5);
   start(1,1);const before=pose()[2];core._th11_key(46,1);step(12);core._th11_key(46,0);step(3);
   if(pose()[2]!==((before+1)%5))throw Error('C must switch Marisa B once, not repeat while held');
   key(46);if(pose()[2]!==((before+2)%5))throw Error('Second C tap failed to switch formation');
   const switched=pose()[2];if(!core._th11_probe_practice_save())throw Error('C replay save failed');
   if(!core._th11_probe_practice_play())throw Error(error());step(180);
   if(pose()[2]!==switched)throw Error('Replay failed to reproduce C formation switches');
   if(!core._th11_return_title())throw Error(error());step(5);
   start(0,1);core._th11_probe_function_pose(-1);key(46);if(pose()[1]>=99)throw Error('C incorrectly enabled gaps on another shot');
   if(!core._th11_return_title())throw Error(error());step(5);
  });
  console.log(locale+': ordinary C input activates both edge gaps, switches Marisa B once and leaves other shots unchanged');
  for(const [name,stage,phases] of [['TH11_ST6_MID1',5,1],['TH11_ST6_BOSS9',5,5],['TH11_ST7_END_S9',6,3],['TH11_ST7_END_S10',6,4]])for(let phase=0;phase<phases;++phase){
   const section=sections.find(s=>s.key===name).id;
   await page.evaluate(({section,stage,phase})=>{
    if(!core._th11_probe_practice_menu(0,0,1))throw Error(error());step(3);const values=[1,stage,section,phase,0,9,0,80,0,0,50000,0,0,1],pointer=core._malloc(values.length*8);
    try{new Float64Array(core.HEAPU8.buffer,pointer,values.length).set(values);if(!core._th11_practice_configure(pointer,values.length))throw Error('Critical phase rejected');}finally{core._free(pointer);}
    step(3);key(44);step(120);if(!state()[15]||state()[2]!==section||state()[3]!==phase)throw Error('Critical section/phase owner changed');if(!core._th11_return_title())throw Error(error());step(3);
   },{section,stage,phase});
  }
  assert.deepEqual(errors,[]);console.log(locale+': original entry/cancel, six shots with boss/chapter warps, Pause-R retry, PRAC save/restore, overlays and all stage-six/Extra special phases passed');await page.close();
 }
}finally{await browser?.close();await new Promise(r=>server.close(r));}
