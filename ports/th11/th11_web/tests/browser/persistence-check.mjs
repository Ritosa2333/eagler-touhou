import assert from 'node:assert/strict';import{createServer}from'node:http';import{readFileSync,mkdirSync,writeFileSync}from'node:fs';import{resolve}from'node:path';import{fileURLToPath}from'node:url';import{createHash}from'node:crypto';
import{launchBrowser}from'../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url)),out=resolve(root,'artifacts/sdl3/browser/persistence');mkdirSync(out,{recursive:true});
const files=new Map(),chunks=[],index={files:[],remote_package_size:0,music:[]};
function data(filename,path){const b=readFileSync(path),start=index.remote_package_size;chunks.push(b);index.remote_package_size+=b.length;index.files.push({filename,start,end:index.remote_package_size});}
data('/th11.dat',resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
for(const n of['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'])data('/fonts/'+n,resolve(root,'assets/sdl-native/fonts',n));
const packed=Buffer.concat(chunks);
for(const n of['managed.mjs','managed.css','keyboard.mjs'])files.set('/runtime/th11/'+n,resolve(root,'sdl-runtime',n));
files.set('/runtime/th11/th11.html',resolve(root,'sdl-runtime/managed.html'));files.set('/runtime/th11/th11.mjs',resolve(root,'artifacts/sdl3/th11-harness.mjs'));files.set('/runtime/th11/th11-harness.wasm',resolve(root,'artifacts/sdl3/th11-harness.wasm'));
const musicFiles=['th11_00','th11_01','th11_14','th11_18','th11_19','th10_17'];
for(const n of musicFiles)files.set('/music/'+n+'.ogg',resolve(root,'assets/sdl-native/music',n+'.ogg'));
const html=`<!doctype html><title>TH11 isolated persistence check</title><style>iframe{width:640px;height:480px;border:0}</style><script>
let requests=0;const pending=new Map();window.ready=false;window.failures=[];
window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await fetch('/data').then(r=>r.arrayBuffer())});
window.addEventListener('message',e=>{const m=e.data;if(e.source!==document.querySelector('iframe')?.contentWindow||e.origin!==location.origin)return;if(m.event==='ready')ready=true;if(m.event==='error')failures.push(m.error);if(pending.has(m.request)){const p=pending.get(m.request);pending.delete(m.request);p(m);}});
window.rpc=m=>new Promise(resolve=>{const request='test'+(++requests);pending.set(request,resolve);document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th11',...m,request},location.origin);});
window.reloadRuntime=()=>{ready=false;document.querySelector('iframe')?.remove();const frame=document.createElement('iframe');frame.src='/runtime/th11/th11.html?managedData=1';document.body.append(frame);};
addEventListener('DOMContentLoaded',reloadRuntime);</script>`;
const server=createServer((req,res)=>{const name=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');
 if(name==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}if(name==='/data'){res.end(packed);return;}if(name==='/runtime/th11/th11.data.json'){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(index));return;}
 if(!files.has(name)){res.writeHead(404).end();return;}res.setHeader('Content-Type',/\.mjs$/.test(name)?'text/javascript':/\.wasm$/.test(name)?'application/wasm':/\.html$/.test(name)?'text/html':/\.css$/.test(name)?'text/css':'application/octet-stream');res.end(readFileSync(files.get(name)));
});
await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;const errors=[];
try{
 browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});const page=await browser.newPage();page.on('pageerror',e=>errors.push(e.stack));await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>ready);
 const rpc=m=>page.evaluate(m=>rpc(m),m),reload=async()=>{await page.evaluate(()=>reloadRuntime());await page.waitForFunction(()=>ready);};
 assert.equal((await rpc({command:'configure',music:'none',resources:musicFiles.map(n=>({path:'/music/'+n+'.ogg',url:'/music/'+n+'.ogg'}))})).ok,true);
 assert.equal((await rpc({command:'launch'})).ok,true);await page.waitForFunction(()=>document.querySelector('iframe').contentWindow.Module._th11_frame()>100);
 const frame=page.frames().find(f=>f.url().includes('th11.html'));await page.screenshot({path:resolve(out,'title.png')});const saved=await frame.evaluate(()=>{const c=Module;c._th11_loop_stop();const tick=(held,n=1)=>{for(let i=0;i<n;++i)if(!c._th11_tick(held))throw Error('title selection failed');};tick(1);tick(0,31);tick(1);tick(0,24);tick(1);tick(0,24);tick(1);tick(0,140);if(c._th11_phase()!==1)throw Error('title did not start a stage');const p=c._malloc(9);c.HEAPU8.set(new TextEncoder().encode('TEST\0'),p);const ok=c._th11_save_replay(1,p);c._free(p);return ok;});assert.equal(saved,1);
 const pause=await frame.evaluate(()=>{const c=Module;const err=()=>{let p=c._th11_error(),s='';while(c.HEAPU8[p])s+=String.fromCharCode(c.HEAPU8[p++]);return s;};const tick=(held=0,n=1)=>{for(let i=0;i<n;++i)if(!c._th11_tick(held))throw Error(err());};const press=k=>{tick(k);tick(0);};const before=c._th11_frame();if(!c._th11_pause())throw Error('pause rejected');tick(0,12);if(c._th11_frame()!==before)throw Error('battle advanced during pause');return {before};});
 await page.screenshot({path:resolve(out,'pause.png')});
 await frame.evaluate(()=>{const c=Module;const tick=(held=0,n=1)=>{for(let i=0;i<n;++i)if(!c._th11_tick(held))throw Error('pause flow failed');};const press=k=>{tick(k);tick(0);};press(32);press(32);press(1);tick(0,31);press(16);press(1);tick(0,31);press(1);tick(0,11);for(let i=0;i<8;++i)press(1);press(1);tick(0,12);press(2);tick(0,14);if(c._th11_phase()!==0)throw Error('pause save did not return to title');tick(0,100);});
 if(process.env.TH11_CHECK_ENDING==='1'){
  await frame.evaluate(()=>{const c=Module;if(!c._th11_probe_stage(6,0,0,1))throw Error('stage6 probe failed');for(let i=0;i<120;++i)if(!c._th11_tick(0))throw Error('stage6 tick failed');if(!c._th11_probe_finish()||!c._th11_tick(0)||c._th11_phase()!==5)throw Error('ending entry failed');});
  let done=false;for(let start=0;start<24000&&!done;start+=200){done=await frame.evaluate(start=>{const c=Module;for(let n=start;n<start+200;++n){if(!c._th11_tick(n%17===0?1:0)){let p=c._th11_error(),s='';while(c.HEAPU8[p])s+=String.fromCharCode(c.HEAPU8[p++]);throw Error(s);}if(c._th11_phase()===0)return true;}return false;},start);if(start===200)await page.screenshot({path:resolve(out,'ending.png')});}assert.ok(done,'ending reaches results');
  await frame.evaluate(()=>{const c=Module,tick=(held=0,n=1)=>{for(let i=0;i<n;++i)if(!c._th11_tick(held))throw Error('ending result flow failed');};tick(0,12);tick(1);tick(0,20);tick(1);tick(0,12);});await page.screenshot({path:resolve(out,'ending-save.png')});
  await frame.evaluate(()=>{const c=Module;for(const held of[1,0,2,0])if(!c._th11_tick(held))throw Error('completed replay save failed');for(let i=0;i<8;++i)if(!c._th11_tick(0))throw Error('completed title return failed');});
 }
 const timing=await page.evaluate(async()=>{const start=performance.now(),first=await rpc({command:'sync'}),middle=performance.now(),second=await rpc({command:'sync'});return {first,second,firstMs:middle-start,unchangedMs:performance.now()-middle};});assert.equal(timing.first.ok,true);assert.equal(timing.second.ok,true);
 const listing=await rpc({command:'list'});assert.deepEqual(listing.files.map(f=>f.path).sort(),['replay/th11_01.rpy','scoreth11.dat','th11.cfg']);
 const originals={};for(const f of listing.files){const r=await rpc({command:'read',path:f.path});assert.equal(r.ok,true);originals[f.path]=r.bytes;}
 await reload();for(const[path,bytes]of Object.entries(originals))assert.deepEqual((await rpc({command:'read',path})).bytes,bytes,'IDB reload '+path);
 for(const path of['scoreth11.dat','th11.cfg','replay/th11_01.rpy','../private.txt'])assert.equal((await rpc({command:'write',path,bytes:[1,2,3]})).ok,false,'reject invalid '+path);
 for(const[path,bytes]of Object.entries(originals))assert.deepEqual((await rpc({command:'read',path})).bytes,bytes,'invalid import preserves '+path);
 const cfg=[...originals['th11.cfg']];cfg[32]=25;cfg[33]=35;assert.equal((await rpc({command:'write',path:'th11.cfg',bytes:cfg})).ok,true);
 assert.equal((await rpc({command:'write',path:'replay/th11_02.rpy',bytes:originals['replay/th11_01.rpy']})).ok,true);
 const demo=Array.from(readFileSync(resolve(root,'reference/assets/demo2.rpy')));assert.equal((await rpc({command:'write',path:'replay/th11_udtest.rpy',bytes:demo})).ok,true);
 await reload();assert.deepEqual((await rpc({command:'read',path:'th11.cfg'})).bytes,cfg);assert.deepEqual((await rpc({command:'read',path:'replay/th11_udtest.rpy'})).bytes,demo);
 assert.equal((await rpc({command:'remove',path:'replay/th11_02.rpy'})).ok,true);await reload();assert.equal((await rpc({command:'list'})).files.some(f=>f.path==='replay/th11_02.rpy'),false);
 assert.equal((await rpc({command:'configure',music:'none',resources:musicFiles.map(n=>({path:'/music/'+n+'.ogg',url:'/music/'+n+'.ogg'}))})).ok,true);
 assert.equal((await rpc({command:'launch'})).ok,true);await page.waitForFunction(()=>document.querySelector('iframe').contentWindow.Module._th11_frame()>100);
 const catalogPlayback=await page.frames().find(f=>f.url().includes('th11.html')).evaluate(extraUnlocked=>{
  const c=Module;c._th11_loop_stop();const tick=(held=0,n=1)=>{for(let i=0;i<n;++i)if(!c._th11_tick(held)){const p=c._th11_error();throw Error(new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p))));}};const press=k=>{tick(k);tick();};
  press(32);press(32);if(extraUnlocked)press(32);press(1);tick(0,40);press(1);tick(0,16);press(1);tick(0,35);
  if(c._th11_phase()!==1)throw Error('Replay catalog did not load selected recording');tick(0,30);return {phase:c._th11_phase(),frame:c._th11_frame()};
 },process.env.TH11_CHECK_ENDING==='1');assert.equal(catalogPlayback.phase,1);
 assert.deepEqual(await page.evaluate(()=>failures),[]);assert.deepEqual(errors,[]);
 const result={passed:true,physicalDevice:false,endingFlow:process.env.TH11_CHECK_ENDING==='1',wasmSha256:createHash('sha256').update(readFileSync(resolve(root,'artifacts/sdl3/th11-harness.wasm'))).digest('hex'),files:listing.files,timing:{firstMs:timing.firstMs,unchangedMs:timing.unchangedMs},reloads:4,invalidImportsPreserved:true,originalReplayImport:true,catalogPlayback};writeFileSync(resolve(out,'report.json'),JSON.stringify(result,null,2));console.log(JSON.stringify(result));
}finally{if(browser)await browser.close();await new Promise(r=>server.close(r));}
