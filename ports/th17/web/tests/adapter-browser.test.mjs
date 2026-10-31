import {test} from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {launchBrowser} from '../../../../tools/browser/launch.mjs';

const repository=resolve(import.meta.dirname,'../../../..');
const build=resolve(process.env.EAGLER_TH17_BUILD||resolve(repository,'ports/th17/build-eagler'));
const data=resolve(process.env.EAGLER_TH17_DATA||resolve(repository,'games/th17/original/th17.dat'));
const output=resolve(repository,'artifacts/validation/th17');

test('TH17 production adapter binds epochs and runs configure, files, input, health and exit', {timeout:180000},async()=>{
 const html=`<!doctype html><style>iframe{display:block;width:960px;height:720px;border:0}</style><button id="start">Start</button><script>
 window.events=[];window.requests=[];
 window.addEventListener('message',e=>{if(e.source===document.querySelector('iframe')?.contentWindow&&e.origin===location.origin)events.push(e.data)});
 window.__eaglerPrepareManagedRuntimeDataV1=async request=>{requests.push(request);if(request.game!=='th17'||request.epoch!==7)throw Error('Wrong DATA owner');return {buffer:await(await fetch('/game.dat')).arrayBuffer()}};
 document.querySelector('button').onclick=()=>{window.__touhouAudioContext=new AudioContext();window.__touhouAudioContext.resume()};
 </script><iframe src="/runtime/th17.html?runtimeEpoch=7&managedData=1&gameGeneration=test"></iframe>`;
 const server=createServer(async(req,res)=>{
  try{
   const path=new URL(req.url,'http://localhost').pathname;
   res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');
   if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
   const local=path.startsWith('/runtime/')?path.slice(9):'';
   const music=path==='/music/th17_01.ogg';
   if(path!=='/game.dat'&&!music&&(!local||local.split('/').some(p=>p==='..'||!p))){res.writeHead(404).end();return;}
   res.setHeader('Content-Type',path==='/game.dat'?'application/octet-stream':({'.mjs':'text/javascript','.wasm':'application/wasm','.html':'text/html','.json':'application/json','.css':'text/css'}[extname(path)]||'application/octet-stream'));
   res.end(await readFile(path==='/game.dat'?data:music?resolve(process.env.EAGLER_TH17_MUSIC||resolve(repository,'games/th17/assets-ogg/music'),'th17_01.ogg'):resolve(build,local)));
  }catch(error){res.writeHead(404).end(String(error));}
 });
 await new Promise(yes=>server.listen(0,'127.0.0.1',yes));
 let browser;
 try{
  browser=await launchBrowser();const page=await browser.newPage({viewport:{width:1024,height:900}});
  const errors=[];page.on('pageerror',e=>errors.push(String(e)));
  await page.goto('http://127.0.0.1:'+server.address().port);
  await page.click('#start');
  await page.waitForFunction(()=>events.some(e=>e.event==='ready'),null,{timeout:90000});
  const identity={protocol:'eagler-touhou/1',game:'th17',epoch:7};let serial=0;
  async function command(command,fields={}){
   const request='test-'+(++serial);
   await page.evaluate(m=>document.querySelector('iframe').contentWindow.postMessage(m,location.origin),{...identity,command,request,...fields});
   await page.waitForFunction(id=>events.some(e=>e.request===id),request,{timeout:30000});
   return page.evaluate(id=>events.find(e=>e.request===id),request);
  }
  assert.equal((await command('launch')).ok,false,'launch must follow configure');
  const before=await page.evaluate(()=>events.length);
  await page.evaluate(m=>document.querySelector('iframe').contentWindow.postMessage(m,location.origin),{...identity,epoch:6,command:'list',request:'retired'});
  await page.waitForTimeout(80);
  assert.equal(await page.evaluate(()=>events.some(e=>e.request==='retired')),false);
  assert.equal(await page.evaluate(()=>events.slice(0).some(e=>e.event==='first-frame')),false);
  assert.equal((await command('configure',{music:'midi'})).ok,false);
  assert.equal((await command('configure',{music:'none',options:{touchSensitivity:301}})).ok,false);
  assert.equal((await command('configure',{music:'none',language:'ja',options:{touchEnabled:true,touchSensitivity:200,alwaysHitbox:true}})).ok,true);
  assert.equal((await command('resources',{resources:[{path:'/th17.dat',url:'/game.dat'}]})).ok,false);
  assert.equal((await command('resources',{resources:[{path:'/music/th17_01.ogg',url:'https://example.invalid/music.ogg'}]})).ok,false);
  assert.equal((await command('read',{path:'../th17.dat'})).ok,false);
  assert.equal((await command('write',{path:'scoreth17.dat',bytes:[1,2,3]})).ok,false);
  assert.equal((await command('launch')).ok,true);
  await page.waitForFunction(()=>events.some(e=>e.event==='first-frame'));
  await page.waitForFunction(()=>events.some(e=>e.event==='error')||(events.some(e=>e.event==='audio-health')&&events.some(e=>e.event==='frame-health')));
  assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
  assert.equal((await command('keyboard',{code:'ArrowDown',down:'yes'})).ok,false);
  assert.equal((await command('direct-touch',{type:'invalid',id:1,x:0,y:0})).ok,false);
  assert.equal((await command('touch-controls',{touchSensitivity:99})).ok,false);
  for(const [name,fields] of [['keyboard',{code:'ArrowDown',down:true}],['keyboard-clear',{}],['touch-controls',{fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:200}],['direct-touch',{type:'down',id:1,x:.5,y:.5}],['touch-cancel',{}],['sync',{}]])assert.equal((await command(name,fields)).ok,true,name);
  const files=await command('list');assert.equal(files.ok,true);assert.ok(files.files.some(f=>f.path==='scoreth17.dat'));
  assert.ok((await command('read',{path:'th17.cfg'})).bytes.length>0);
  assert.equal((await command('write',{path:'th17.cfg',bytes:[1]})).ok,false,'live writes are rejected');
  await page.locator('iframe').evaluate(frame=>frame.contentWindow.dispatchEvent(new Event('blur')));
  const keyboardNeutral=await page.locator('iframe').evaluate(frame=>frame.contentWindow.__eaglerKeyboardInputBridge.keyDown('ArrowDown'));
  assert.equal(keyboardNeutral,false);
  await mkdir(output,{recursive:true});
  const frame=page.frames().find(f=>f.url().includes('/runtime/'));
  await frame.waitForFunction(()=>Module._th17_frame()>160);
  await page.screenshot({path:resolve(output,'title.png')});
  for(let i=0;i<2;i++){
   await command('keyboard',{code:'Escape',down:true});await page.waitForTimeout(80);
   await command('keyboard',{code:'Escape',down:false});await page.waitForTimeout(80);
  }
  await page.waitForFunction(()=>events.some(e=>e.event==='exit'));
  assert.notEqual(await page.evaluate(()=>__touhouAudioContext.state),'closed');
  const events=await page.evaluate(()=>window.events);
  assert.ok(events.length>before);assert.ok(events.every(e=>e.protocol==='eagler-touhou/1'&&e.game==='th17'&&e.epoch===7));
  assert.ok(!events.some(e=>e.event==='error'),JSON.stringify(events.filter(e=>e.event==='error')));
  assert.deepEqual(errors,[]);
  assert.equal(await page.evaluate(()=>requests.length),1);
  const savedConfig=await command('read',{path:'th17.cfg'});
  await page.reload();await page.click('#start');await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
  assert.deepEqual((await command('read',{path:'th17.cfg'})).bytes,savedConfig.bytes,'IDBFS persists across documents');
  assert.equal((await command('configure',{music:'ogg',resources:[{path:'/music/th17_01.ogg',url:'/music/th17_01.ogg'}]})).ok,true);
  assert.equal((await command('launch')).ok,true);
  await page.waitForFunction(()=>events.some(e=>e.event==='error')||events.some(e=>e.event==='audio-health'&&e.musicEnabled));
  assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
  const oggEvents=await page.evaluate(()=>events);
  await page.reload();await page.click('#start');await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
  const replay=await readFile(process.env.EAGLER_TH17_REPLAY||resolve(repository,'ports/th17/web/reference/assets/demo1.rpy'));
  assert.equal((await command('write',{path:'replay/th17_ud0001.rpy',bytes:[...replay]})).ok,true);
  assert.equal((await command('configure',{music:'none',options:{replayPath:'replay/th17_ud0001.rpy',alwaysHitbox:true}})).ok,true);
  assert.equal((await command('launch')).ok,true);
  const replayFrame=page.frames().find(f=>f.url().includes('/runtime/'));
  await replayFrame.waitForFunction(()=>Module._th17_phase()===1&&Module._th17_frame()>200);
  assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
  await page.screenshot({path:resolve(output,'replay.png')});
  await writeFile(resolve(output,'adapter-browser.json'),JSON.stringify({passed:true,scope:'production Wasm, same-origin Host fixture, desktop Chromium software WebGL2; none/OGG; IDBFS reload; imported original demo startup',events,oggEvents,replayEvents:await page.evaluate(()=>events),errors},null,2)+'\n');
 }finally{await browser?.close();await new Promise(yes=>server.close(yes));}
});
