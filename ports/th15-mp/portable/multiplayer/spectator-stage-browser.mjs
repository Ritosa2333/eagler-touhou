import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,readdir} from 'node:fs/promises';
import {resolve,extname,dirname} from 'node:path';
import {createRequire} from 'node:module';
import {pathToFileURL} from 'node:url';
const root=resolve(import.meta.dirname,'../..');
const launcher=resolve(process.env.TH15_LAUNCHER_ROOT);
const {default:puppeteer}=await import(pathToFileURL(createRequire(resolve(launcher,'package.json')).resolve('puppeteer-core')));
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
const runtime=resolve(root,'th15_web/artifacts/sdl-multiplayer/spectator-stage');
const futureTicks=600;
if(!process.env.TH15_TEST_ARCHIVE)throw Error('Set TH15_TEST_ARCHIVE');
const archive=await readFile(process.env.TH15_TEST_ARCHIVE);
const fontRoot=resolve(process.env.TH15_TEST_FONTS||resolve(dirname(process.env.TH15_TEST_ARCHIVE),'fonts'));
const fonts=(await readdir(fontRoot)).filter(p=>p==='cp932.bin');
const sharedFonts=resolve(process.env.TH15_TEST_SHARED_FONTS||resolve(root,'../dist/ui-rewrite-fixed-20261010/shared'));
const html=`<!doctype html><canvas id="canvas" width="640" height="480"></canvas><script type="module">
import createModule from '/th15.mjs';
window.moduleReady=(async()=>{window.Module=await createModule({canvas:document.querySelector('canvas'),eaglerOptions:{netplaySpectator:true},printErr:console.error});Module.FS.writeFile('/th15.dat',new Uint8Array(await(await fetch('/data')).arrayBuffer()));Module.FS.mkdirTree('/fonts');for(const name of ${JSON.stringify(fonts)})Module.FS.writeFile('/fonts/'+name,new Uint8Array(await(await fetch('/fonts/'+name)).arrayBuffer()));for(const name of ['msgothic.ttc','unifont.otf'])Module.FS.writeFile('/'+name,new Uint8Array(await(await fetch('/shared/'+name)).arrayBuffer()));Module._th15_music_enabled(0);if(!Module._th15_initialize()){const p=Module._th15_error();throw Error('Initialize failed: '+new TextDecoder().decode(Module.HEAPU8.subarray(p,Module.HEAPU8.indexOf(0,p))));}return true;})();
</script>`;
const server=createServer(async(req,res)=>{try{
 const path=new URL(req.url,'http://localhost').pathname;
 if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
 if(path==='/data'){res.end(archive);return;}
 if(path==='/shared/msgothic.ttc'||path==='/shared/unifont.otf'){res.end(await readFile(resolve(sharedFonts,path.slice(8))));return;}
 if(path.startsWith('/fonts/')&&fonts.includes(path.slice(7))){res.end(await readFile(resolve(fontRoot,path.slice(7))));return;}
 if(!/^\/(?:th15\.mjs|th15\.wasm)$/.test(path)){res.writeHead(404).end();return;}
 res.setHeader('Content-Type',extname(path)==='.mjs'?'application/javascript':'application/wasm');res.end(await readFile(resolve(runtime,path.slice(1))));
}catch(e){res.writeHead(500).end(String(e));}});
await new Promise(yes=>server.listen(0,'127.0.0.1',yes));
let browser;const reports=[];
try{
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--autoplay-policy=no-user-gesture-required','--use-angle=swiftshader','--enable-unsafe-swiftshader','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 const page=await browser.newPage();page.on('pageerror',e=>console.log('pageerror:',String(e)));await page.goto('http://127.0.0.1:'+server.address().port);await page.evaluate(()=>moduleReady);
 for(const count of [2,3]){
  const result=await page.evaluate(({count,futureTicks})=>{
   const m=Module,readError=()=>{const p=m._th15_spectator_lab_error();return new TextDecoder().decode(m.HEAPU8.subarray(p,m.HEAPU8.indexOf(0,p)));};
   const check=(ok,label)=>{if(!ok)throw Error(label+': '+readError());};
   const state=()=>{const p=m._th15_spectator_lab_state()>>>2;return Array.from(m.HEAPU32.subarray(p,p+16));};
   const step=held=>check(m._th15_spectator_lab_step(1,held),'tick');
   check(m._th15_spectator_lab_begin(count,1),'begin');const report=[];
   for(const stage of [1,2]){
    let n=0;const boundaryTrace=[];for(;n<1000&&!m._th15_spectator_lab_boundary();n++){const p=m._th15_spectator_lab_boundary_state()>>>2,trace=Array.from(m.HEAPU32.subarray(p,p+12));if(trace[0]===stage&&trace[1]<40)boundaryTrace.push(trace);step(0);}
    check(m._th15_spectator_lab_boundary(),'boundary '+stage+' '+JSON.stringify({state:state(),trace:boundaryTrace}));
    const before=state(),counts=()=>{const p=m._th15_spectator_lab_animation_counts()>>>2;return Array.from(m.HEAPU32.subarray(p,p+3));},inventory=()=>{const p=m._th15_spectator_lab_animation_inventory();return JSON.parse(new TextDecoder().decode(m.HEAPU8.subarray(p,m.HEAPU8.indexOf(0,p))));},animationsBefore=counts(),inventoryBefore=inventory();check(m._th15_spectator_lab_capture(),'capture '+stage);
    const logic=()=>Array.from(m.HEAPU8.subarray(m._th15_spectator_lab_logic_bytes(),m._th15_spectator_lab_logic_bytes()+m._th15_spectator_lab_logic_size()));const logicBefore=logic();
    const frame=m._th15_spectator_lab_frame(),size=m._th15_spectator_lab_size();const bytes=Array.from(m.HEAPU8.subarray(m._th15_spectator_lab_bytes(),m._th15_spectator_lab_bytes()+size));
    const expected=[],expectedInventory=[];for(let i=0;i<futureTicks;i++){step(i<30?0x41:1);expected.push(state());expectedInventory.push(inventory());}
    check(m._th15_spectator_lab_restore(),'restore '+stage);const after=state(),animationsAfter=counts();
    if(JSON.stringify(before)!==JSON.stringify(after)){const logicAfter=logic(),logicDiff=logicBefore.flatMap((value,offset)=>value===logicAfter[offset]?[]:[{offset,before:value,after:logicAfter[offset]}]);throw Error('restore state differs '+JSON.stringify({before,after,logicDiff:logicDiff.slice(0,80),logicLength:[logicBefore.length,logicAfter.length],animationsBefore,animationsAfter,inventoryBefore,inventoryAfter:inventory()}));}
    for(let i=0;i<futureTicks;i++){step(i<30?0x41:1);const actual=state();if(JSON.stringify(actual)!==JSON.stringify(expected[i]))throw Error('future tick differs '+JSON.stringify({stage,tick:i,expected:expected[i],actual,expectedInventory:expectedInventory[i],actualInventory:inventory()}));}
    report.push({stage,count,frame,before,after,size,bytes,expected,animationsBefore,animationsAfter,futureTicks,maxEnemies:Math.max(...expected.map(s=>s[8])),maxBullets:Math.max(...expected.map(s=>s[9])),maxItems:Math.max(...expected.map(s=>s[10]))});
    if(stage===1){check(m._th15_spectator_lab_distinct_power(),'native per-seat power');check(m._th15_spectator_lab_next(),'native next stage');}
   }
   return report;
  },{count,futureTicks});
  const receiver=await browser.newPage();receiver.on('pageerror',e=>console.log('receiver pageerror:',String(e)));await receiver.goto('http://127.0.0.1:'+server.address().port);await receiver.evaluate(()=>moduleReady);
  await receiver.evaluate(count=>{if(!Module._th15_spectator_lab_begin(count,1))throw Error('Independent receiver begin failed');},count);
  for(const row of result){
   await receiver.evaluate(row=>{
    const m=Module,error=()=>{const p=m._th15_spectator_lab_error();return new TextDecoder().decode(m.HEAPU8.subarray(p,m.HEAPU8.indexOf(0,p)));};
    const p=m._malloc(row.size);m.HEAPU8.set(row.bytes,p);try{if(!m._th15_spectator_lab_import(p,row.size,row.frame))throw Error('Independent import: '+error());}finally{m._free(p);}
    const state=()=>{const p=m._th15_spectator_lab_state()>>>2;return Array.from(m.HEAPU32.subarray(p,p+16));};
    if(JSON.stringify(state())!==JSON.stringify(row.before))throw Error('Independent initial state differs '+JSON.stringify({expected:row.before,actual:state()}));
    for(let i=0;i<row.expected.length;i++){
     if(!m._th15_spectator_lab_step(1,i<30?0x41:1))throw Error('Independent tick: '+error());const actual=state();
     if(JSON.stringify(actual)!==JSON.stringify(row.expected[i]))throw Error('Independent future tick differs '+JSON.stringify({stage:row.stage,tick:i,expected:row.expected[i],actual}));
    }
   },row);
   await writeFile(resolve(runtime,`${count}p-stage${row.stage}.bin`),new Uint8Array(row.bytes));delete row.bytes;delete row.expected;row.independentWasm=true;reports.push(row);console.log(JSON.stringify(row));
  }
  await receiver.close();
 }
 assert.equal(reports.length,4);await writeFile(resolve(runtime,'native-restoration.json'),JSON.stringify({status:'PASS',reports},null,2)+'\n');
}catch(e){await writeFile(resolve(runtime,'native-restoration.failure.txt'),String(e.stack||e));throw e;}
finally{await browser?.close();await new Promise(yes=>server.close(yes));}
