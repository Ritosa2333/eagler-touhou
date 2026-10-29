// Original local assets, production WASM, Launcher RPC and the shared relay.
import assert from 'node:assert/strict';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {createServer} from 'node:http';
import {spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {resolve,extname,sep} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {DATA_SCHEMA,DATA_MAGIC,DATA_LAYOUT_SPEC} from '../src/managed-data.mjs';
const root=resolve(import.meta.dirname,'..'),repo=resolve(root,'../..'),assets=resolve(process.env.TH12_ASSETS??resolve(root,'../th12/site/assets'));
const fonts=resolve(process.env.TH12_FONTS??resolve(assets,'../shared'));
const difficulty=Number(process.env.TH12_MULTIPLAYER_DIFFICULTY??1),slowPresentation=process.env.TH12_MULTIPLAYER_SLOW_PRESENTATION==='1';
assert([0,1,2,3,4].includes(difficulty));
const runtime=resolve(process.env.TH12_RUNTIME??resolve(root,'build-eagler')),out=resolve(root,'artifacts/sdl-browser');await mkdir(out,{recursive:true});
const sha=b=>createHash('sha256').update(b).digest('hex'),entries=[],chunks=[];let offset=0;
for(const name of (await readdir(assets)).sort())if(/^[a-zA-Z0-9_-]+\.(?:json|png|anm|ecl|std|sht|msg|wav|rpy)$/.test(name)){
 const b=await readFile(resolve(assets,name));entries.push({path:'assets/'+name,offset,bytes:b.length,sha256:sha(b)});chunks.push(b);offset+=b.length;
}
const manifest=Buffer.from(JSON.stringify({schema:DATA_SCHEMA,game:'th12',layout:'sha256-'+sha(JSON.stringify(DATA_LAYOUT_SPEC)),files:entries})),header=Buffer.alloc(12);header.write(DATA_MAGIC);header.writeUInt32LE(manifest.length,8);
const data=Buffer.concat([header,manifest,...chunks]);chunks.length=0;
const html=`<!doctype html><style>body{margin:0;background:#222}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();let data;
window.__eaglerPrepareManagedRuntimeDataV1=async()=>{data??=fetch('/data').then(r=>r.arrayBuffer());const buffer=await data;return {buffer,bytes:buffer.byteLength};};
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.addRuntime=s=>{const f=document.createElement('iframe');f.id='p'+s;f.src='/runtime/th12.html?hosted=1&managedData=1&runtimeVariant=normal&runtimeEpoch='+(s+1);document.body.append(f);};
window.rpc=(s,command,fields={})=>new Promise((yes,no)=>{const f=document.querySelector('#p'+s),request=crypto.randomUUID(),timer=setTimeout(()=>no(Error(command+' timed out')),90000);function receive(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',receive);e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',receive);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th12',epoch:s+1,command,request,...fields},location.origin);});
</script>`;
const server=createServer(async(req,res)=>{try{const url=new URL(req.url,'http://localhost');if(url.pathname==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}if(url.pathname==='/data'){res.end(data);return;}
 if(url.pathname==='/favicon.ico'){res.writeHead(204).end();return;}
 if(url.pathname.startsWith('/shared/')){res.end(await readFile(resolve(fonts,url.pathname.split('/').at(-1))));return;}
 const p=resolve(runtime,decodeURIComponent(url.pathname).replace(/^\/runtime\//,''));if(!p.startsWith(runtime+sep))throw Error('invalid path');res.setHeader('Content-Type',({'.mjs':'text/javascript','.js':'text/javascript','.html':'text/html','.css':'text/css','.json':'application/json','.wasm':'application/wasm'})[extname(p)]??'application/octet-stream');res.end(await readFile(p));
 }catch(e){res.writeHead(404).end(e.message);}});await new Promise(r=>server.listen(0,'127.0.0.1',r));
const browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
try{
 const page=await browser.newPage({viewport:{width:960,height:800}}),errors=[];
 page.on('pageerror',e=>errors.push(e.message));
 await page.route('**/game.css',route=>route.abort());
 page.on('console',m=>{if(m.type()==='error')console.log('BROWSER',m.text());});
 await page.goto('http://127.0.0.1:'+server.address().port);await page.evaluate(()=>addRuntime(0));
 await page.waitForFunction(()=>ready.size===1,null,{timeout:90000});
 await page.evaluate(async()=>{await rpc(0,'configure',{music:'none',language:'ja',sharedResources:[{path:'/unifont.otf',url:'/shared/unifont.otf'},{path:'/msgothic.ttc',url:'/shared/msgothic.ttc'}],options:{thpracEnabled:true,touchEnabled:true,touchMovementMode:'touch-unlimited',touchSensitivity:100}});await rpc(0,'launch');});
 const frame=page.frames().find(f=>f.url().includes('runtimeEpoch='));
 const state=()=>frame.evaluate(()=>__th12Runtime.state());
 assert.equal(await frame.locator('.toolbar').isVisible(),false);assert.equal(await frame.locator('footer').isVisible(),false);
 async function wait(predicate){const end=Date.now()+90000;while(Date.now()<end){const v=await state();if(predicate(v))return v;const events=await page.evaluate(()=>window.events.filter(e=>e.event==='error'));assert.deepEqual(events,[]);await page.waitForTimeout(50);}throw Error('scene timeout '+JSON.stringify(await state()));}
 async function press(code){await page.evaluate(code=>rpc(0,'keyboard',{code,down:true,inputSource:'physical'}),code);await page.waitForTimeout(90);await page.evaluate(code=>rpc(0,'keyboard',{code,down:false,inputSource:'physical'}),code);await page.waitForTimeout(80);}
 await wait(s=>s.scene==='title'&&s.menuReady);await page.screenshot({path:resolve(out,'title.png')});
 for(const scene of ['difficulty','character','shot','playing']){await press('KeyZ');await wait(s=>s.scene===scene&&(s.menuReady||scene==='playing'));}
 await page.waitForTimeout(1800);const playing=await state();assert.equal(playing.stage,1);assert(playing.fps>0&&playing.fps<90);
 if(process.env.TH12_PROFILE){
   await frame.evaluate(()=>{window.glCalls={getError:0,copyTexSubImage2D:0};for(const name of Object.keys(glCalls)){const original=WebGL2RenderingContext.prototype[name];WebGL2RenderingContext.prototype[name]=function(...args){glCalls[name]++;return original.apply(this,args);};}});
   const cdp=await page.context().newCDPSession(page);await cdp.send('Profiler.enable');await cdp.send('Profiler.start');
   const samples=[];for(let i=0;i<120;i++){await page.waitForTimeout(100);samples.push(await state());}
   const {profile}=await cdp.send('Profiler.stop');await writeFile(resolve(out,'cpu-profile.json'),JSON.stringify(profile));await writeFile(resolve(out,'frame-samples.json'),JSON.stringify(samples));await writeFile(resolve(out,'gl-calls.json'),JSON.stringify(await frame.evaluate(()=>glCalls)));
 }

 // Exercise the public touch protocol inside a portrait iframe with letterboxing.
 await page.locator('#p0').evaluate(f=>{f.style.width='400px';f.style.height='800px';});
 await page.waitForTimeout(200);await wait(s=>s.ready);
 const contact=await frame.evaluate(()=>{const r=document.querySelector('#screen').getBoundingClientRect();return {x:(r.left+r.width/2)/innerWidth,y:(r.top+r.height*.8)/innerHeight,moveY:(r.top+r.height*.8-60)/innerHeight,dy:Math.round(60*480/r.height*128)};});
 const beforeTouch=await state();
 await page.evaluate(c=>rpc(0,'direct-touch',{type:'down',id:301,x:c.x,y:c.y}),contact);
 await page.evaluate(c=>rpc(0,'direct-touch',{type:'move',id:301,x:c.x,y:c.moveY}),contact);
 await page.waitForTimeout(150);const afterTouch=await state();
 await page.evaluate(c=>rpc(0,'direct-touch',{type:'up',id:301,x:c.x,y:c.moveY}),contact);
 assert(Math.abs(afterTouch.y-(beforeTouch.y-contact.dy))<=2,'portrait upward drag must preserve the actual canvas displacement');
 await page.evaluate(()=>rpc(0,'touch-controls',{fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:1,joystickX:0,joystickY:0}));
 await wait(s=>s.scene==='paused'&&s.menuReady);await press('Enter');await wait(s=>s.scene==='playing');
 console.log('TOUCH_ESC_AND_PORTRAIT_DRAG_PASS',{beforeTouch,afterTouch,contact});
 await page.locator('#p0').evaluate(f=>{f.style.width='640px';f.style.height='480px';});await page.waitForTimeout(200);
 await press('F12');await wait(s=>s.capturing);await page.screenshot({path:resolve(out,'thprac.png')});
 const closedVertices=(await state()).practiceVertices;
 const click=await frame.evaluate(()=>{const r=document.querySelector('#screen').getBoundingClientRect();return {x:r.left+20*r.width/640,y:r.top+42*r.height/480};});
 await page.evaluate(p=>rpc(0,'thprac-mouse',{type:'move',...p}),click);
 await page.evaluate(p=>rpc(0,'thprac-mouse',{type:'down',...p}),click);await page.waitForTimeout(100);
 await page.evaluate(p=>rpc(0,'thprac-mouse',{type:'up',...p}),click);await page.waitForTimeout(150);
 const openVertices=(await state()).practiceVertices;assert(openVertices>closedVertices+100,'hosted THPrac mouse must expand the speed settings');
 await page.screenshot({path:resolve(out,'thprac-mouse-expanded.png')});console.log('THPRAC_HOSTED_MOUSE_PASS',{closedVertices,openVertices});

 await press('F12');await wait(s=>!s.capturing);
 const fullscreen=[];
 for(let i=0;i<2;i++){
   await frame.evaluate(async()=>{if(document.fullscreenElement)await document.exitFullscreen();else await document.querySelector('#screen').requestFullscreen();});
   await page.waitForTimeout(500);fullscreen.push(await state());
 }
 await press('Escape');await wait(s=>s.scene==='paused'&&s.menuReady);await page.screenshot({path:resolve(out,'pause.png')});
 await press('ArrowDown');await press('ArrowDown');await press('Enter');await wait(s=>s.scene==='pause-confirm'&&s.menuReady);
 await press('ArrowUp');await press('Enter');await wait(s=>s.scene==='replay-save'&&s.menuReady);
 await press('Enter');await wait(s=>s.scene==='name'&&s.menuReady);await press('Enter');
 const listing=await page.evaluate(()=>rpc(0,'list'));const replay=listing.files.find(f=>f.path.endsWith('th12_01.rpy'));assert(replay?.size>176);
 const info=await page.evaluate(path=>rpc(0,'playback',{action:'inspect',path}),replay.path);assert.equal(info.description.mode,'story');
 await page.evaluate(path=>rpc(0,'playback',{action:'open',path}),replay.path);await wait(s=>s.scene==='replay-playing');
 await page.evaluate(()=>rpc(0,'playback',{action:'pause'}));await page.waitForTimeout(2200);const frozen=await state();assert.equal(frozen.fps,0);
 await page.evaluate(()=>rpc(0,'playback',{action:'seek',value:60}));await wait(s=>!s.seeking&&s.cursor===60);
 await page.evaluate(()=>rpc(0,'playback',{action:'play'}));await wait(s=>s.cursor>70);
 assert.deepEqual(errors,[]);assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
 const result={playing,fullscreen,pausedReplay: frozen,replay:info.description,errors};await writeFile(resolve(out,'result.json'),JSON.stringify(result,null,2));console.log('PASS',JSON.stringify(result));
}finally{await browser.close();server.closeAllConnections();await new Promise(r=>server.close(r));}
