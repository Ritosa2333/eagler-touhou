import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep,basename} from 'node:path';
import {createHash} from 'node:crypto';
import {launchBrowser} from '../../../../tools/browser/launch.mjs';

// A development-probe build uses the real hosted input, RAF, thprac and draw
// paths. Release packaging rejects these probes; original game data stays local.
const root=resolve(process.env.EAGLER_TH17_BUILD);
const probeModule=process.env.TH17_SINGLE_MODULE?resolve(process.env.TH17_SINGLE_MODULE):resolve(root,'th17.mjs');
const probeWasm=probeModule.replace(/\.mjs$/,'.wasm');
const out=resolve(process.env.TH17_SINGLE_EVIDENCE||'artifacts/th17-single-player');
const assets={'/game.dat':await readFile(process.env.EAGLER_TH17_DATA)};
for(const name of ['msgothic.ttc','unifont.otf'])
 assets['/'+name]=await readFile(resolve(process.env.EAGLER_TH17_SHARED,name));
const html=`<!doctype html><style>iframe{width:1280px;height:960px;border:0}</style><script>
window.events=[];addEventListener('message',e=>events.push(e.data));
window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/game.dat')).arrayBuffer()});
window.rpc=(command,fields={})=>new Promise((r,j)=>{
 const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);j(Error(command+' timeout'));},30000);
 function receive(e){if(e.data.request===request){clearTimeout(timer);removeEventListener('message',receive);e.data.ok?r(e.data):j(Error(e.data.error));}}
 addEventListener('message',receive);document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th17',epoch:7,request,command,...fields},location.origin);
});</script><iframe src='/runtime/th17.html?runtimeEpoch=7&managedData=1'></iframe>`;
const server=createServer(async(req,res)=>{
 try{
  const path=new URL(req.url,'http://localhost').pathname;
  res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  if(path==='/'){res.setHeader('Content-Type','text/html');return res.end(html);}
  if(assets[path])return res.end(assets[path]);
  assert.ok(path.startsWith('/runtime/'));
  const file=path==='/runtime/th17.mjs'?probeModule:path==='/runtime/'+basename(probeWasm)?probeWasm:resolve(root,path.slice(9));
  assert.ok(file===probeModule||file===probeWasm||file.startsWith(root+sep));
  res.setHeader('Content-Type',({'.mjs':'text/javascript','.wasm':'application/wasm','.html':'text/html'})[extname(file)]||'application/octet-stream');
  res.end(await readFile(file));
 }catch{res.writeHead(404).end();}
});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
let browser;const errors=[],checks=[],transitions=[];
await mkdir(out,{recursive:true});
try{
 browser=await launchBrowser({args:['--autoplay-policy=no-user-gesture-required','--enable-gpu','--use-gl=angle','--use-angle=d3d11']});
 const page=await browser.newPage({viewport:{width:1320,height:1000}});page.on('pageerror',e=>errors.push(String(e)));
 await page.goto('http://127.0.0.1:'+server.address().port);
 await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
 await page.evaluate(async()=>{
  await rpc('configure',{music:'none',language:'ja',sharedResources:['/msgothic.ttc','/unifont.otf'].map(path=>({path,url:path})),
   options:{thpracEnabled:true,thpracLocale:'zh-CN',touchEnabled:false,renderScale:2}});
  await rpc('launch');
 });
 const frame=page.frames().at(-1);
 await frame.waitForFunction(()=>Module._th17_frame()>100);
 assert.equal(await frame.evaluate(()=>typeof Module._th17_probe_practice_state),'function','Use a development-probe build for single-player regressions');
 const canvas=await frame.evaluate(()=>[Module.canvas.width,Module.canvas.height]);assert.deepEqual(canvas,[1280,960]);
 const state=async(name,count)=>frame.evaluate(({name,count})=>Array.from(Module.HEAP32.subarray(Module[name]()>>>2,(Module[name]()>>>2)+count)),{name,count});
 const practice=()=>state('_th17_probe_practice_state',8),pause=()=>state('_th17_probe_pause_state',7);
 assert.equal((await practice())[0],1);
 const press=async code=>{
  await page.evaluate(code=>rpc('keyboard',{code,down:true}),code);await page.waitForTimeout(65);
  await page.evaluate(code=>rpc('keyboard',{code,down:false}),code);await page.waitForTimeout(100);
 };
 const controls=async fields=>page.evaluate(fields=>rpc('touch-controls',{fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,...fields}),fields);
 for(const mobile of [false,true]){
  await controls({thpracMobilePanelsEnabled:mobile,thpracMobilePanelsVisible:false});
  await press('F12');assert.equal((await practice())[1],1,'keyboard F12 opens the desktop window with hidden mobile panels');
  await page.screenshot({path:resolve(out,mobile?'f12-with-mobile.png':'f12.png')});
  await press('Escape');assert.equal((await practice())[1],0);
  const before=await page.evaluate(()=>events.filter(e=>e.event==='thprac-menu').length);
  await press('Backspace');
  assert.equal(await page.evaluate(before=>events.filter(e=>e.event==='thprac-menu').slice(before).at(-1)?.open,before),true);
  await page.screenshot({path:resolve(out,mobile?'backspace-with-mobile.png':'backspace.png')});
  await press('Backspace');
  assert.equal(await page.evaluate(()=>events.filter(e=>e.event==='thprac-menu').at(-1)?.open),false);
  checks.push({mobile,desktopAdvanced:true,desktopCheats:true});
 }
 // Native DOM and hosted commands must reach the same virtual-key map.
 await frame.locator('canvas').focus();await page.keyboard.press('F12',{delay:65});await page.waitForTimeout(100);
 assert.equal((await practice())[1],1);await press('Escape');
 for(let attempt=0;await frame.evaluate(()=>Module._th17_phase())===0;attempt++){
  assert.ok(attempt<8,'story starts through the original title selections');await press('KeyZ');await page.waitForTimeout(900);
 }
 await frame.waitForFunction(()=>Module._th17_phase()===1&&Module.HEAP32[(Module._th17_probe_transition_state()>>>2)+1]>40);
 for(const mobile of [false,true]){
  await controls({thpracMobilePanelsEnabled:mobile,thpracMobilePanelsVisible:false});
  await press('Tab');assert.equal((await practice())[2],1,'Tab displays desktop information');
  await page.screenshot({path:resolve(out,mobile?'tab-with-mobile.png':'tab.png')});
  await press('Tab');assert.equal((await practice())[2],0);
 }
 await press('Escape');await frame.waitForFunction(()=>Module.HEAP32[(Module._th17_probe_pause_state()>>>2)+1]===6);
 assert.equal((await pause())[2],0);
 await press('KeyR');assert.equal((await pause())[2],4,'R selects Restart rather than the current item');
 await frame.waitForFunction(()=>{const p=Module._th17_probe_pause_state()>>>2;return Module.HEAP32[p+1]===7&&Module.HEAP32[p+6]>=30;});
 await press('ArrowUp');await press('KeyZ');
 await frame.waitForFunction(()=>Module._th17_phase()===1&&Module.HEAP32[(Module._th17_probe_transition_state()>>>2)+1]<40,null,{timeout:30000});
 checks.push({restartSelected:true,restarted:true});
 await frame.waitForFunction(()=>Module.HEAP32[(Module._th17_probe_transition_state()>>>2)+1]>40);
 await frame.evaluate(()=>Module._th17_loop_stop());
 assert.equal(await frame.evaluate(()=>Module._th17_probe_finish_stage()),1);
 for(let tick=0;tick<220;tick++){
  assert.equal(await frame.evaluate(()=>Module._th17_probe_tick()),1);
  const value=await state('_th17_probe_transition_state',8);transitions.push(value);
  if(value[0]===2&&value[4]){
   assert.equal(value[2],60,'incoming background waits for the stage-clear notice');
   assert.equal(value[3],30,'outgoing background remains visible during the notice');
   assert.equal(value[5]&8,0,'outgoing scene cannot retire during the notice');
  }
 }
 assert.ok(transitions.some(v=>v[0]===2&&v[4]));
 assert.ok(transitions.some(v=>v[0]===2&&!v[4]&&v[3]>0&&v[3]<30));
 assert.ok(transitions.some(v=>v[0]===2&&v[3]===-1&&v[1]>=30),'outgoing background releases after its fade');
 assert.deepEqual(errors,[]);assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
 const wasm=createHash('sha256').update(await readFile(probeWasm)).digest('hex');
 await writeFile(resolve(out,'single-player-report.json'),JSON.stringify({passed:true,wasm,canvas,checks,transitions},null,2)+'\n');
 console.log('TH17 desktop hotkeys, mobile coexistence, R restart and stage-clear background timing passed');
}finally{await browser?.close();server.closeAllConnections();await new Promise(r=>server.close(r));}
