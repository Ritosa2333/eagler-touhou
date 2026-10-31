// Runs the actual TH08 normal/MP shells, native WASM keyboard table and Launcher mapper.
// Original game data and published code artifacts are explicit local inputs.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {build} from 'esbuild';
import {createHash} from 'node:crypto';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
const repo=resolve(import.meta.dirname,'../../..'),code=resolve(process.env.TH08_KEYBOARD_RUNTIME_ROOT??''),data=process.env.TH08_TEST_DATA,fonts=process.env.TH08_SHARED_FONTS;
assert.ok(process.env.TH08_KEYBOARD_RUNTIME_ROOT&&data&&fonts,'Set TH08_KEYBOARD_RUNTIME_ROOT (normal/ and multiplayer/), TH08_TEST_DATA and TH08_SHARED_FONTS');
const out=resolve(process.env.TH08_KEYBOARD_OUTPUT??resolve(repo,'artifacts/th08-keyboard-browser'));await mkdir(out,{recursive:true});
const bundle=await build({absWorkingDir:repo,entryPoints:['launcher/app/services/player-keyboard.ts'],bundle:true,write:false,format:'esm',platform:'browser'}),mapper=bundle.outputFiles[0].contents,archive=await readFile(data);
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#222}iframe{width:640px;height:480px;border:0}</style><iframe id="runtime"></iframe><script type="module">
import {createPlayerKeyboard} from '/mapper.mjs';
window.events=[];window.launched=false;window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const buffer=await(await fetch('/data')).arrayBuffer();return {buffer,bytes:buffer.byteLength};};
window.rpc=(command,fields={})=>new Promise((yes,no)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',listen);no(Error(command+' timeout'));},90000);function listen(e){if(e.source===runtime.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',listen);e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',listen);runtime.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th08',epoch:1,command,request,...fields},location.origin);});
addEventListener('message',e=>{if(e.source===runtime.contentWindow)events.push(e.data);});
window.bindMapper=()=>{const input=()=>({target:runtime.contentWindow,targetOrigin:location.origin,protocol:'eagler-touhou/1',game:'th08',epoch:1,launched:window.launched,ready:true,spectator:false});window.owner=createPlayerKeyboard({runtime:{getInputContext:input,getSnapshot:()=>({ready:true,firstFrame:true}),getMidiEventContext:()=>({target:runtime.contentWindow,document:runtime.contentDocument,game:'th08',epoch:1,music:'none'}),postInput:(command,fields)=>runtime.contentWindow.__eaglerKeyboardInputBridge.submit({...input(),command,...fields}),subscribe:()=>()=>{}},window,document,frame:runtime,playerOpen:()=>true,launcherOwnsTarget:t=>!!t?.closest?.('input,textarea,select,[contenteditable="true"]'),releaseHeldTouchFire(){},cancelTouchFunction(){},toggleFullscreen(){},isPlayerFullscreen:()=>false,android:false});};
window.start=(mode,variant)=>{runtime.src='/runtime/'+mode+'/'+variant+'/th08.html?hosted=1&managedData=1&runtimeEpoch=1&runtimeVariant='+variant+'&manual=1';};
</script>`;
const server=createServer(async(req,res)=>{try{const path=decodeURIComponent(new URL(req.url,'http://localhost').pathname);let bytes,type;
 if(path==='/'){bytes=html;type='text/html';}else if(path==='/mapper.mjs'){bytes=mapper;type='text/javascript';}else if(path==='/data'){bytes=archive;}else if(path.startsWith('/shared/')){const name=path.slice(8);assert.ok(['msgothic.ttc','unifont.otf'].includes(name));bytes=await readFile(resolve(fonts,name));}else if(path==='/favicon.ico'){res.writeHead(204).end();return;}else{
  const match=path.match(/^\/runtime\/(baseline|patched)\/(normal|multiplayer)\/(.+)$/);assert.ok(match,'Unexpected URL '+path);const [,mode,variant,name]=match,base=resolve(code,variant),file=resolve(base,name);assert.ok(file.startsWith(base+sep));
  if(mode==='patched'&&['shell.mjs','directory-keyboard.mjs'].includes(name))bytes=await readFile(resolve(repo,'ports',variant==='normal'?'th08':'th08-mp','web/sdl-runtime',name));else bytes=await readFile(file);
 }
 res.setHeader('Content-Type',type??({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.json':'application/json','.wasm':'application/wasm'})[extname(path)]??'application/octet-stream');res.end(bytes);
}catch(e){res.writeHead(500).end(String(e));}});await new Promise(r=>server.listen(0,'127.0.0.1',r));
let browser;const reports=[];
try{browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
for(const variant of ['normal','multiplayer'])for(const mode of ['baseline','patched']){
 const context=await browser.newContext(),page=await context.newPage(),errors=[];page.on('pageerror',e=>errors.push(e.message));
 await context.addInitScript(()=>{const instantiate=WebAssembly.instantiateStreaming;WebAssembly.instantiateStreaming=async(...args)=>{const r=await instantiate(...args);if(!r.instance.exports.sdl_game_open)return r;const native=r.instance.exports,core={...native};core.sdl_game_open=(...args)=>{window.keyboardApp=native.sdl_game_open(...args);return window.keyboardApp;};window.keyboardCore=core;return {...r,instance:{exports:core}};};});
 try{
  await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>!!window.start);await page.evaluate(({mode,variant})=>start(mode,variant),{mode,variant});await page.waitForFunction(()=>events.some(e=>e.event==='ready'),null,{timeout:90000});await page.evaluate(()=>bindMapper());
  await page.evaluate(async variant=>{await rpc('configure',{music:'none',language:'ja',sharedResources:[{path:'/msgothic.ttc',url:'/shared/msgothic.ttc'},{path:'/unifont.otf',url:'/shared/unifont.otf'}],options:{thpracEnabled:false,touchEnabled:false,netplayMode:'off'}});await rpc(variant==='normal'?'launch':'prepare-room');window.launched=true;owner.rebind();},variant);
  const frame=page.frames().find(f=>f.url().includes('/runtime/'));
  const checks=await frame.evaluate(()=>{
   // Retire menu calculations while retaining the SDL keyboard poll and native
   // key table. Repeated Z/arrow probes must not navigate the title or quit.
   const core=keyboardCore,app=keyboardApp,output=[];core.sdl_loop_stop();core.close(app);
   const step=()=>{const result=core.sdl_loop_tick(app,1/60,0);if(result!==1)throw Error('Expected retired calculations with live SDL polling, got '+result);};
   const state=vk=>{step();return new Uint8Array(core.memory.buffer,core.keyboard(app),256)[vk];};
   const key=(code,down,extra={})=>window.dispatchEvent(new KeyboardEvent(down?'keydown':'keyup',{code,bubbles:true,cancelable:true,...extra}));
   for(const [code,vk] of [['ArrowUp',38],['ShiftLeft',16],['KeyZ',90]]){
    for(const [reason,extra] of [['composition',{isComposing:true}],['shortcut',{altKey:true}]]){
     core.sdl_keys_clear();key(code,true,extra);const rejected=state(vk);key(code,false);const released=state(vk);output.push({code,reason,rejected,released});
    }
    const input=document.createElement('input');document.body.append(input);core.sdl_keys_clear();const editing=new KeyboardEvent('keydown',{code,bubbles:true,cancelable:true});input.dispatchEvent(editing);const rejected=state(vk);input.dispatchEvent(new KeyboardEvent('keyup',{code,bubbles:true,cancelable:true}));output.push({code,reason:'editing',rejected,released:state(vk),typingPrevented:editing.defaultPrevented});input.remove();
    core.sdl_keys_clear();for(let i=0;i<30;i++){key(code,true);if(!state(vk))throw Error('Swallowed fresh '+code);key(code,false);if(state(vk))throw Error('Stuck ordinary '+code);}
   }
   core.sdl_keys_clear();key('ArrowUp',true);state(38);window.dispatchEvent(new Event('blur'));if(state(38))throw Error('Blur left native input');key('ArrowUp',true,{repeat:true});if(state(38))throw Error('Old repeat rearmed after blur');key('ArrowUp',false);key('ArrowUp',true);if(!state(38))throw Error('Fresh input swallowed after blur');key('ArrowUp',false);core.sdl_keys_clear();key('ShiftLeft',true);key('ShiftRight',true);key('ShiftLeft',false);const otherShift=state(16);key('ShiftRight',false);const allShiftReleased=state(16);output.push({otherShift,allShiftReleased});
   const mapping=window.__eaglerKeyboardMapping,handoffs=[];
   // Keep the production Launcher port, but exercise attach/detach before UP.
   core.sdl_keys_clear();delete window.__eaglerKeyboardMapping;key('ArrowUp',true);const rawDown=state(38);window.__eaglerKeyboardMapping=mapping;key('ArrowUp',false);const attachedUp=state(38);handoffs.push({change:'attach',down:rawDown,released:attachedUp});
   core.sdl_keys_clear();key('ShiftLeft',true);const mappedDown=state(16);delete window.__eaglerKeyboardMapping;key('ShiftLeft',false);const detachedUp=state(16);handoffs.push({change:'detach',down:mappedDown,released:detachedUp});window.__eaglerKeyboardMapping=mapping;
   core.sdl_keys_clear();key('KeyZ',true);const heldZ=state(90);window.__eaglerKeyboardMapping={...mapping,epoch:2};key('KeyZ',false);handoffs.push({change:'stale',down:heldZ,released:state(90)});window.__eaglerKeyboardMapping=mapping;
   core.sdl_keys_clear();key('ArrowUp',true);const heldUp=state(38);window.__eaglerKeyboardMapping={...mapping,map(e,down,source,profile){return mapping.map({...e,code:e.code==='ArrowUp'?'ArrowRight':e.code},down,source,profile);}};key('ArrowUp',false);handoffs.push({change:'replace-remapping',down:heldUp,released:state(38)});window.__eaglerKeyboardMapping=mapping;core.sdl_keys_clear();
   output.push({handoffs});return output;
  });
  await frame.locator('canvas').focus();
  const trusted=[];
  for(const [code,vk] of [['ArrowUp',38],['ShiftLeft',16],['KeyZ',90]]){
   await page.keyboard.down(code);const down=await frame.evaluate(vk=>{const c=keyboardCore;if(c.sdl_loop_tick(keyboardApp,1/60,0)!==1)throw Error('Native poll');return new Uint8Array(c.memory.buffer,c.keyboard(keyboardApp),256)[vk];},vk);
   await page.keyboard.up(code);const up=await frame.evaluate(vk=>{const c=keyboardCore;if(c.sdl_loop_tick(keyboardApp,1/60,0)!==1)throw Error('Native poll');return new Uint8Array(c.memory.buffer,c.keyboard(keyboardApp),256)[vk];},vk);
   assert.deepEqual([down,up],[128,0],variant+' trusted '+code);trusted.push({code,down,up});
  }
  for(const row of checks.filter(x=>x.code)){assert.equal(row.rejected,mode==='baseline'?128:0,variant+' '+mode+' rejected '+row.code);assert.equal(row.released,mode==='baseline'?128:0,variant+' '+mode+' release '+row.code);}
  assert.deepEqual(checks.find(x=>'otherShift' in x),{otherShift:128,allShiftReleased:0});for(const handoff of checks.at(-1).handoffs){assert.equal(handoff.down,128,variant+' '+mode+' '+handoff.change);assert.equal(handoff.released,mode==='baseline'?128:0,variant+' '+mode+' '+handoff.change+' released');}assert.deepEqual(errors,[]);assert.equal(await frame.locator('#error').innerText(),'');
  const wasm=await readFile(resolve(code,variant,'th08-sdl.wasm'));reports.push({variant,mode,checks,ordinaryPressReleaseCycles:90,trustedBrowserKeys:trusted,wasmSha256:createHash('sha256').update(wasm).digest('hex'),errors});console.log('PASS '+variant+' '+mode+': '+(mode==='baseline'?'reproduced persistent IME-key hold':'native release clean, no swallowed fresh inputs'));
 }finally{await context.close();}
}
await writeFile(resolve(out,'result.json'),JSON.stringify({passed:true,actualShellAndNative:true,scope:'Production shell listeners, Launcher mapper, SDL poll and native key table; game calculations retired after native initialization',reports},null,2)+'\n');
}finally{await browser?.close();await new Promise(r=>server.close(r));}
