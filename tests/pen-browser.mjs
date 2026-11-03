import {createServer} from 'node:http';
import {readFile,mkdir} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {inspectReplay} from '../web/replay.mjs';
import {ORIGINAL_BGM,AKYU_BGM,musicFileName} from '../web/audio-layout.mjs';
const {unzipSync}=createRequire(new URL('../../../launcher/package.json',import.meta.url))('fflate');
import {launchBrowser} from '../../../tools/browser/launch.mjs';
const root=resolve(import.meta.dirname,'..'),runtime=resolve(process.env.TH05_TEST_RUNTIME??resolve(root,process.env.TH05_BROWSER_TESTS==='1'?'artifacts/test-runtime':'artifacts/runtime')),content=resolve(process.env.TH05_TEST_CONTENT??resolve(root,'artifacts/content/th05.data'));
const font=resolve(process.env.TH05_TEST_FONT??resolve(root,'../../dependencies/unifont-15.1.05/unifont-15.1.05.otf'));
const fixture=`<!doctype html><html><body style="margin:0"><script>
window.events=[];window.responses=new Map();window.serial=0;
window.__eaglerPrepareManagedRuntimeDataV1=async request=>{if(request.game!=='th05'||request.epoch!==7)throw Error('identity');return {buffer:await (await fetch('/th05.data')).arrayBuffer()};};
addEventListener('message',e=>{if(e.origin!==location.origin)return;events.push(e.data);if(e.data.request&&responses.has(e.data.request)){responses.get(e.data.request)(e.data);responses.delete(e.data.request);}});
window.command=(command,payload={})=>new Promise(resolve=>{const request='r'+(++serial);responses.set(request,resolve);document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th05',epoch:7,command,request,...payload},location.origin);});
</script><iframe style="border:0;width:640px;height:480px" src="/runtime/th05.html?runtimeEpoch=7&managedData=1"></iframe></body></html>`;
const server=createServer(async(req,res)=>{if(req.url==='/favicon.ico'){res.writeHead(204);res.end();return;}try{let body,type;if(req.url==='/'){body=fixture;type='text/html';}else if(req.url==='/th05.data'){body=await readFile(content);type='application/octet-stream';}else if(req.url==='/language.json'){const zip=await readFile(resolve(process.env.TH05_TEST_ASSETS??resolve(root,'artifacts'),'languages/language/lang_zh-hans.zip')),files=unzipSync(zip);body=JSON.stringify({language:'lang_zh-hans',url:'/language.zip',bytes:zip.length,manifest:JSON.parse(new TextDecoder().decode(files['manifest.json'])),files:[{path:'/thcrap/th05/chs.dat',bytes:Array.from(files['thcrap/th05/chs.dat'])}]});type='application/json';}else if(req.url.startsWith('/music/')){const name=req.url.slice(7);if(!/^[a-z0-9._-]+$/.test(name))throw Error('path');body=await readFile(resolve(process.env.TH05_TEST_ASSETS??resolve(root,'artifacts'),'content/music',name));type='audio/ogg';}else if(req.url==='/font.ttf'){body=await readFile(font);type='font/ttf';}else{const name=decodeURIComponent(new URL(req.url,'http://test').pathname).replace(/^\/runtime\//,'');if(name.includes('..')||name.startsWith('/'))throw Error('path');body=await readFile(resolve(runtime,name));type=({'.mjs':'text/javascript','.html':'text/html','.wasm':'application/wasm','.json':'application/json'})[extname(name)]??'application/octet-stream';}res.writeHead(200,{'Content-Type':type});res.end(body);}catch(error){res.writeHead(404);res.end(error.message);}});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const browser=await launchBrowser();const logs=[],report={};
try{
 report.runtime=JSON.parse(await readFile(resolve(runtime,'build.json'),'utf8'));report.runtime={wasmSha256:report.runtime.sha256,builtAt:report.runtime.builtAt,diagnostic:report.runtime.diagnostic};
 const page=await browser.newPage({viewport:{width:660,height:500}});page.on('pageerror',e=>logs.push('pageerror: '+e.message));page.on('console',e=>{if(e.type()==='error')logs.push(e.text());});
 async function launch(enabled,locale='ja-JP',language='ja',audioMode='none',source='original',viewer=false){await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>events.some(e=>e.event==='ready'),null,{timeout:30000});
  const config=await page.evaluate(async({enabled,locale,language,audioMode,source,viewer})=>{const pack=language==='lang_zh-hans'?await (await fetch('/language.json')).json():null;if(pack)for(const file of pack.files)file.bytes=new Uint8Array(file.bytes);return command('configure',{music:audioMode==='none'?'none':'ogg',language,...(pack?{runtimePack:pack}:{}),sharedResources:[{path:'/data/gothic.ttf',url:'/font.ttf'}],options:{replayViewer:viewer,musicSource:source,oggDecodeMode:audioMode==='full'?'full':'stream',thpracEnabled:enabled,thpracLocale:locale,alwaysHitbox:true,touchEnabled:true,touchSensitivity:150,touchMovementMode:'touch-unlimited'}});},{enabled,locale,language,audioMode,source,viewer});assert.equal(config.ok,true,JSON.stringify(config));
  const result=await page.evaluate(()=>command('launch'));assert.equal(result.ok,true,JSON.stringify(result));await page.waitForFunction(()=>events.some(e=>e.event==='first-frame'),null,{timeout:15000});return page.frames().find(f=>f.url().includes('/runtime/'));}
 let frame=await launch(false);
 report.vanilla=await frame.evaluate(()=>({enabled:Module._th05_practice_enabled(),scene:Module._th05_scene(),ticks:Module._th05_ticks()}));assert.equal(report.vanilla.enabled,0);
 await frame.evaluate(()=>{Module._th05_key(112,1);Module._th05_step();Module._th05_key(112,0);});assert.equal(await frame.evaluate(()=>Module._th05_practice_tool(0)),0);
 // Advance the real opening, title and character scenes through their input.
 await frame.evaluate(()=>{for(let i=0;i<220&&Module._th05_scene()===1;i++)Module._th05_step();for(let i=0;i<250;i++)Module._th05_step();});
 await page.screenshot({path:resolve(root,'artifacts/title-vanilla.png')});
 const press=async(key,frames=2)=>frame.evaluate(({key,frames})=>{Module._th05_key(key,1);for(let i=0;i<frames;i++)Module._th05_step();Module._th05_key(key,0);Module._th05_step();},{key,frames});
 await press(90);await frame.evaluate(()=>{for(let i=0;i<90;i++)Module._th05_step();});await press(90);await frame.evaluate(()=>{for(let i=0;i<110;i++)Module._th05_step();});
 report.gameplay=await frame.evaluate(()=>({scene:Module._th05_scene(),x:Module._th05_player_x(),y:Module._th05_player_y(),ticks:Module._th05_ticks()}));assert.equal(report.gameplay.scene,4,JSON.stringify(report.gameplay));
 await frame.evaluate(()=>{for(let n=0;n<240;n++)Module._th05_step();});
 // A key focused inside the Runtime must pass through the Launcher's mapper.
 const mappedMovement=await frame.evaluate(()=>{
  const bridge=window.__eaglerKeyboardInputBridge;
  window.__eaglerKeyboardMapping={
   schema:bridge.schema,protocol:bridge.protocol,game:bridge.game,epoch:bridge.epoch,origin:bridge.origin,
   supports:code=>code==='KeyJ',clear:()=>[],
   map:(event,down)=>event.code==='KeyJ'?[{code:'ArrowRight',key:'ArrowRight',keyCode:39,location:0,down}]:null,
  };
  try {
   const canvas=document.querySelector('canvas'),before=Module._th05_player_x();
   canvas.dispatchEvent(new KeyboardEvent('keydown',{code:'KeyJ',key:'j',bubbles:true}));
   for(let i=0;i<4;i++)Module._th05_step();
   const held=Module._th05_player_x();
   canvas.dispatchEvent(new KeyboardEvent('keyup',{code:'KeyJ',key:'j',bubbles:true}));
   for(let i=0;i<4;i++)Module._th05_step();
   return {before,held,released:Module._th05_player_x()};
  } finally {delete window.__eaglerKeyboardMapping;}
 });
 assert.ok(mappedMovement.held>mappedMovement.before,JSON.stringify(mappedMovement));
 assert.equal(mappedMovement.released,mappedMovement.held,'Remapped movement releases on keyup');
 const pen=await page.context().newCDPSession(page),surface=await frame.locator('canvas').boundingBox();
 const start=await frame.evaluate(()=>Module._th05_player_x());
 const stylus=(type,x,down=false)=>pen.send('Input.dispatchMouseEvent',{type,pointerType:'pen',x:surface.x+x,y:surface.y+300,button:type==='mouseMoved'?'none':'left',buttons:down?1:0});
 await stylus('mouseMoved',200);await page.waitForTimeout(100);assert.equal(await frame.evaluate(()=>Module._th05_player_x()),start);
 await stylus('mousePressed',200,true);await stylus('mouseMoved',230,true);await page.waitForTimeout(180);await stylus('mouseReleased',230);await page.waitForTimeout(100);
 const after=await frame.evaluate(()=>Module._th05_player_x());assert.ok(after>start&&after-start<100,JSON.stringify({start,after}));
 await stylus('mouseMoved',280);await page.waitForTimeout(100);assert.equal(await frame.evaluate(()=>Module._th05_player_x()),after);
 await page.mouse.move(surface.x+200,surface.y+300);await page.mouse.down();await page.mouse.move(surface.x+230,surface.y+300);await page.waitForTimeout(100);await page.mouse.up();assert.equal(await frame.evaluate(()=>Module._th05_player_x()),after,'Ordinary mouse remains a UI pointer');
 console.log(JSON.stringify({passed:true,start,after,hoverStable:true,mouseUnchanged:true}));assert.deepEqual(logs,[]);
}finally{await browser.close();await new Promise(r=>server.close(r));}
