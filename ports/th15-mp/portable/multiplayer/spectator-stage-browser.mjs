// Native Replay stage entries plus their activation input, in independent WASMs.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../../tools/browser/launch.mjs';
const root=resolve(import.meta.dirname,'../..');
const runtime=resolve(root,'web',process.env.TH15_OUTPUT||'artifacts/sdl-multiplayer','spectator-stage');
for(const name of ['TH15_TEST_ARCHIVE','TH15_TEST_CP932_FONT','TH15_TEST_SHARED_FONTS'])assert.ok(process.env[name],'Set '+name);
const data=await readFile(process.env.TH15_TEST_ARCHIVE);
const resources=new Map([['/data',data],['/fonts/cp932.bin',await readFile(process.env.TH15_TEST_CP932_FONT)]]);
for(const name of ['msgothic.ttc','unifont.otf'])resources.set('/shared/'+name,await readFile(resolve(process.env.TH15_TEST_SHARED_FONTS,name)));
const html=`<!doctype html><canvas id="canvas" width="640" height="480"></canvas><script type="module">
import createModule from '/th15.mjs';
window.moduleReady=(async()=>{window.Module=await createModule({canvas:document.querySelector('canvas'),eaglerOptions:{netplaySpectator:true},printErr:console.error});
const m=Module;m.FS.writeFile('/th15.dat',new Uint8Array(await(await fetch('/data')).arrayBuffer()));m.FS.mkdirTree('/fonts');
for(const [path,url] of [['/fonts/cp932.bin','/fonts/cp932.bin'],['/msgothic.ttc','/shared/msgothic.ttc'],['/unifont.otf','/shared/unifont.otf']])m.FS.writeFile(path,new Uint8Array(await(await fetch(url)).arrayBuffer()));
m._th15_music_enabled(0);if(!m._th15_initialize()){const p=m._th15_error();throw Error('Native initialization failed: '+new TextDecoder().decode(m.HEAPU8.subarray(p,m.HEAPU8.indexOf(0,p))));}return true;})();</script>`;
const server=createServer(async(req,res)=>{try{
 const path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
 if(resources.has(path)){res.end(resources.get(path));return;}
 if(!['/th15.mjs','/th15.wasm'].includes(path)){res.writeHead(404).end();return;}
 res.setHeader('Content-Type',path.endsWith('.mjs')?'application/javascript':'application/wasm');res.end(await readFile(resolve(runtime,path.slice(1))));
}catch(error){res.writeHead(500).end(String(error));}});
await new Promise(yes=>server.listen(0,'127.0.0.1',yes));let browser;const reports=[];
try{
 browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
 for(const count of [2,3]){
  const source=await browser.newPage();await source.goto('http://127.0.0.1:'+server.address().port);await source.evaluate(()=>moduleReady);
  await source.evaluate(count=>{if(!Module._th15_spectator_lab_begin(count,1))throw Error('Source begin failed');},count);
  for(const stage of [1,2]){
   const row=await source.evaluate(({stage,count})=>{
    const m=Module,error=()=>{const p=m._th15_spectator_lab_error();return new TextDecoder().decode(m.HEAPU8.subarray(p,m.HEAPU8.indexOf(0,p)));};
    const check=(ok,label)=>{if(!ok)throw Error(label+': '+error());};
    const state=()=>{const p=m._th15_spectator_lab_state()>>>2;return Array.from(m.HEAPU32.subarray(p,p+16));};
    let ready=false;for(let i=0;i<1000;++i){
     check(m._th15_spectator_lab_step(1,0),'source entrance');const value=state();
     if(value[1]===stage&&value[2]===1&&m._th15_spectator_lab_capture()){ready=true;break;}
    }
    check(ready,'native activation '+stage);const initial=state(),initialLogic=Array.from(m.HEAPU8.subarray(m._th15_spectator_lab_logic_bytes(),m._th15_spectator_lab_logic_bytes()+m._th15_spectator_lab_logic_size())),entry=m._th15_spectator_lab_entry_frame();
    const size=m._th15_spectator_lab_size();const bytes=Array.from(m.HEAPU8.subarray(m._th15_spectator_lab_bytes(),m._th15_spectator_lab_bytes()+size));
    const expected=[];for(let i=0;i<600;++i){check(m._th15_spectator_lab_step(1,i<30?0x41:1),'source input');expected.push(state());}
    return {stage,count,entry,size,bytes,initial,initialLogic,expected};
   },{stage,count});
   assert.ok(row.size<=4096);
   const viewer=await browser.newPage();await viewer.goto('http://127.0.0.1:'+server.address().port);await viewer.evaluate(()=>moduleReady);
   await viewer.evaluate(row=>{
    const m=Module,error=()=>{const p=m._th15_spectator_lab_error();return new TextDecoder().decode(m.HEAPU8.subarray(p,m.HEAPU8.indexOf(0,p)));};
    if(!m._th15_spectator_lab_begin(row.count,1))throw Error('Viewer begin failed');
    const pointer=m._malloc(row.size);m.HEAPU8.set(row.bytes,pointer);
    try{if(!m._th15_spectator_lab_import(pointer,row.size,row.entry))throw Error('Native Replay import: '+error());}finally{m._free(pointer);}
    const state=()=>{const p=m._th15_spectator_lab_state()>>>2;return Array.from(m.HEAPU32.subarray(p,p+16));};
    // Runtime/window ages and visual presentation clocks have independent origins.
    // Compare the semantic world, native stage clock, script RNG and entity counts.
    const fields=[0,1,2,4,5,8,9,10];
    const compare=(expected,tick)=>{const actual=state();if(fields.some(i=>actual[i]!==expected[i])){const logic=Array.from(m.HEAPU8.subarray(m._th15_spectator_lab_logic_bytes(),m._th15_spectator_lab_logic_bytes()+m._th15_spectator_lab_logic_size()));const diff=row.initialLogic.flatMap((v,i)=>v===logic[i]?[]:[{offset:i,expected:v,actual:logic[i]}]).slice(0,40);throw Error('Streaming Replay divergence '+JSON.stringify({stage:row.stage,tick,expected,actual,diff}));}};
    if(!m._th15_spectator_lab_step(1,0))throw Error('Activation input: '+error());compare(row.initial,0);
    for(let i=0;i<row.expected.length;++i){if(!m._th15_spectator_lab_step(1,i<30?0x41:1))throw Error('Viewer input: '+error());compare(row.expected[i],i+1);}
   },row);
   await viewer.close();delete row.bytes;delete row.expected;reports.push(row);
   console.log('PASS native streaming Replay '+count+'P stage '+stage+' | 601 matching inputs | '+row.size+' bytes');
   if(stage===1)await source.evaluate(()=>{if(!Module._th15_spectator_lab_distinct_power()||!Module._th15_spectator_lab_next())throw Error('Native stage transition failed');});
  }
  await source.close();
 }
 await writeFile(resolve(runtime,'streaming-replay.json'),JSON.stringify({status:'PASS',reports},null,2)+'\n');
}catch(error){await writeFile(resolve(runtime,'streaming-replay.failure.txt'),String(error.stack||error));throw error;}
finally{await browser?.close();await new Promise(yes=>server.close(yes));}
